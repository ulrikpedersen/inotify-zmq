#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#ifdef __linux__
  #include <sys/inotify.h>
#else
  #include "inotifymock.h"
#endif
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "zmq.hpp"
#include "InotifyEventZmq.h"

#define BUF_SIZE (1024*1024)
void process_event(struct inotify_event* event, zmq::socket_t& socket);


void process_event(struct inotify_event* event, zmq::socket_t& socket)
{
  static const boost::regex name_regex(".*h5");
  boost::posix_time::ptime timestamp(boost::posix_time::microsec_clock::local_time());
  std::string name("");
  if (event->len > 0)
  {
    name = event->name;
    if (boost::regex_match(name, name_regex))
    {
      std::stringstream ss;
      ss << "{\"name\": \"" << name << "\", \"timestamp\": \""
         << boost::posix_time::to_iso_extended_string(timestamp) << "\"}";
      std::string json_msg(ss.str());
      std::cout << json_msg << std::endl;

      zmq::message_t message(json_msg.size());
      memcpy(message.data(), json_msg.c_str(), json_msg.size());
      socket.send(message, ZMQ_NOBLOCK);
    }
  }
}

int main() {
  int return_code = 0;
  int status = 0;
  std::cout << "Hello, World!" << std::endl;
  zmq::context_t context (1);

  int inotify_fd, wd, j;
  char events_buf[BUF_SIZE] __attribute__ ((aligned(8)));
  ssize_t read_count;
  struct inotify_event *event;

  inotify_fd = inotify_init();
  if (inotify_fd == -1) {
    std::cerr << "ERROR: inotify_init() failed. code: " << inotify_fd << std::endl;
    return -1;
  }

  //  Socket to send messages on
  zmq::socket_t sender(context, ZMQ_PUSH);
  sender.bind("tcp://*:9988");

  const char * pathname = ".";
  wd = inotify_add_watch(inotify_fd, pathname, IN_MOVE);
  if (wd == -1)
  {
    std::cerr << "ERROR: Unable to start inotify watch on path: \"" << pathname
              << "\" errno: " << errno << " " << strerror(errno) << std::endl;
    return -1;
  }

  bool carry_on = true;
  while(carry_on) {                                  /* Read events forever */
    read_count = read(inotify_fd, events_buf, BUF_SIZE);
    if (read_count == 0)
    {
      std::cerr << "ERROR: read() from inotify fd returned 0!" << std::endl;
      carry_on = false;
      continue;
    }

    if (read_count == -1)
    {
      std::cerr << "ERROR: read() failed. errno=" << errno << " " << strerror(errno) << std::endl;
      return_code = -1;
      carry_on = false;
      continue;
    }

    //std::cout << "Read " << read_count << " bytes from inotify fd" << std::endl;

    /* Process all of the events in buffer returned by read() */
    for (char * ptr = events_buf; ptr < events_buf + read_count; ) {
      event = (struct inotify_event *) ptr;
      std::cout << *event << std::endl;
      process_event(event, sender);
      ptr += sizeof(struct inotify_event) + event->len;
    }
  }

  std::cout << "Closing down inotify watch" << std::endl;
  status = inotify_rm_watch(inotify_fd, wd);
  if (status == -1)
  {
    std::cerr << "ERROR: inotify_rm_watch() failed. errno=" << errno << " " << strerror(errno) << std::endl;
    return_code = -1;
  }

  std::cout << "Closing down ZMQ push socket" << std::endl;
  sender.close();

  return return_code;
}

