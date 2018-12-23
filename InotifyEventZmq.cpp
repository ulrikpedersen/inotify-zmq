//
// Created by Ulrik K Pedersen on 2018-12-22.
//
#include <iostream>
#include <unistd.h>

#ifdef __linux__
#include <sys/inotify.h>
#else
#include "inotifymock.h"
#endif
#include "InotifyEventZmq.h"
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>


InotifyEventZmq::InotifyEventZmq(zmq::context_t& context) :
inotify_fd(inotify_init()),
zmq_socket(context, ZMQ_PUSH),
inotify_wd(0),
watch_dir_name(".")
{
  if (this->inotify_wd <= 0)
  {
    std::cerr << "ERROR: inotify_init() failed. code: " << this->inotify_fd << std::endl;
  }
}

InotifyEventZmq::~InotifyEventZmq()
{
  zmq_socket.close();
  if (inotify_wd > 0 && inotify_fd > 0)
  {
    inotify_rm_watch(this->inotify_fd, this->inotify_wd);
  }
  if (inotify_fd > 0)
  {
    close(inotify_fd);
  }
}

void InotifyEventZmq::handle_inotify_event()
{
  size_t read_count;
  read_count = read(inotify_fd, events_buf, InotifyEventZmq::buffer_size);
  boost::posix_time::ptime timestamp(boost::posix_time::microsec_clock::local_time());
  if (read_count == 0)
  {
    std::cerr << "ERROR: read() from inotify fd returned 0!" << std::endl;
    return;
  }

  if (read_count == -1)
  {
    std::cerr << "ERROR: read() failed. errno=" << errno << " " << strerror(errno) << std::endl;
    return;
  }

  //std::cout << "Read " << read_count << " bytes from inotify fd" << std::endl;

  /* Process all of the events in buffer returned by read() */
  for (char * ptr = events_buf; ptr < events_buf + read_count; )
  {
    struct inotify_event *event;
    event = (struct inotify_event *) ptr;
    std::cout << *event << std::endl;
    process_event(*event, timestamp);
    ptr += sizeof(struct inotify_event) + event->len;
  }
}

void InotifyEventZmq::watch_dir(const std::string &pathname)
{
  if (inotify_wd > 0 && inotify_fd > 0)
  {
    inotify_rm_watch(this->inotify_fd, this->inotify_wd);
    this->inotify_wd = -1;
  }
  this->inotify_wd = inotify_add_watch(this->inotify_fd, pathname.c_str(), IN_MOVED_TO);
  if (this->inotify_wd <= 0)
  {
    std::cerr << "ERROR: Unable to start inotify watch on path: \"" << pathname
              << "\" errno: " << errno << " " << strerror(errno) << std::endl;
  }
}

void InotifyEventZmq::process_event(const struct inotify_event &event, const boost::posix_time::ptime& timestamp)
{
  std::string name("");
  if (event.len > 0)
  {
    name = event.name;
    if (boost::regex_match(name, this->filename_pattern_regex))
    {
      std::stringstream ss;
      ss << "{\"name\": \"" << name << "\", \"timestamp\": \""
         << boost::posix_time::to_iso_extended_string(timestamp) << "\"}";
      std::string json_msg(ss.str());
      std::cout << json_msg << std::endl;

      zmq::message_t message(json_msg.size());
      memcpy(message.data(), json_msg.c_str(), json_msg.size());
      this->zmq_socket.send(message, ZMQ_NOBLOCK);
    }
  }
}

void InotifyEventZmq::set_filename_pattern_regex(const std::string &filename_pattern_regex) {
  this->filename_pattern_regex = boost::regex(filename_pattern_regex);
}
