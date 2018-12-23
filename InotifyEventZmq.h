//
// Created by Ulrik K Pedersen on 2018-12-22.
//

#ifndef FILETOZMQ_INOTIFYEVENTZMQ_H
#define FILETOZMQ_INOTIFYEVENTZMQ_H

#include <string>
#include <boost/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


#include "zmq.hpp"

class InotifyEventZmq {
public:
  InotifyEventZmq(zmq::context_t& context);
  void handle_inotify_event();
  void watch_dir(const std::string& pathname);
  void process_event(const struct inotify_event &event, const boost::posix_time::ptime& timestamp);
  void set_filename_pattern_regex(const std::string &filename_pattern_regex);
  int get_file_descriptor() { return inotify_fd; }

  virtual ~InotifyEventZmq();

private:
  int inotify_fd; // file descriptor
  int inotify_wd; // watch descriptor
  boost::regex filename_pattern_regex;
  std::string watch_dir_name;
  zmq::socket_t zmq_socket;

  static const size_t buffer_size = (1024 * 1024); // 1MB buffer size should be plenty
  char events_buf[buffer_size] __attribute__ ((aligned(8)));
};


#endif //FILETOZMQ_INOTIFYEVENTZMQ_H
