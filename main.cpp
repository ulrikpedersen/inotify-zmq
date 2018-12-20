#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/inotify.h>

#include "zmq.hpp"

#define BUF_SIZE (1024*1024)
void process_event(struct inotify_event* event, zmq::socket_t& socket);

std::ostream &operator<<(std::ostream &os, inotify_event const &ievent) {
  os << "{"; // Start JSON

  // Cookie
  os << " \"cookie\": " << ievent.cookie;

  // Mask
  os << ", \"mask\": [" << ievent.mask;
  if (ievent.mask & IN_ACCESS)        os << ", \"IN_ACCESS\"";
  if (ievent.mask & IN_ATTRIB)        os << ", \"IN_ATTRIB\"";
  if (ievent.mask & IN_CLOSE_NOWRITE) os << ", \"IN_CLOSE_NOWRITE\"";
  if (ievent.mask & IN_CLOSE_WRITE)   os << ", \"IN_CLOSE_WRITE\"";
  if (ievent.mask & IN_CREATE)        os << ", \"IN_CREATE\"";
  if (ievent.mask & IN_DELETE)        os << ", \"IN_DELETE\"";
  if (ievent.mask & IN_DELETE_SELF)   os << ", \"IN_DELETE_SELF\"";
  if (ievent.mask & IN_IGNORED)       os << ", \"IN_IGNORED\"";
  if (ievent.mask & IN_ISDIR)         os << ", \"IN_ISDIR\"";
  if (ievent.mask & IN_MODIFY)        os << ", \"IN_MODIFY\"";
  if (ievent.mask & IN_MOVE_SELF)     os << ", \"IN_MOVE_SELF\"";
  if (ievent.mask & IN_MOVED_FROM)    os << ", \"IN_MOVED_FROM\"";
  if (ievent.mask & IN_MOVED_TO)      os << ", \"IN_MOVED_TO\"";
  if (ievent.mask & IN_OPEN)          os << ", \"IN_OPEN\"";
  if (ievent.mask & IN_Q_OVERFLOW)    os << ", \"IN_Q_OVERFLOW\"";
  if (ievent.mask & IN_UNMOUNT)       os << ", \"IN_UNMOUNT\"";
  os << "]";

  // dir/file name
  if (ievent.len > 0) {
    os << ", \"name\": \"" << ievent.name << "\"";
  }

  os << " }"; // End JSON
  return os;
}

void process_event(struct inotify_event* event, zmq::socket_t& socket)
{
  std::string name("");
  if (event->len > 0)
  {
    std::stringstream ss; ss << *event;
    std::string event_json(ss.str());

    zmq::message_t message(event_json.size());
    memcpy(message.data(), event_json.c_str(), event_json.size());
    //std::cout << message << std::endl;
    socket.send(message, ZMQ_NOBLOCK);
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

