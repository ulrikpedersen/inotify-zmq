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
#include <poll.h>

#include "zmq.hpp"
#include "InotifyEventZmq.h"
#include "FilePushZmq.h"


int inotify_eventloop()
{
  int return_code = 0;
  int status = 0;
  std::cout << "Hello, World!" << std::endl;
  zmq::context_t context (1);

  InotifyEventZmq iez(context);

  const char * pathname = ".";
  iez.watch_dir(pathname);

  bool carry_on = true;
  while(carry_on)
  {                                  /* Read events forever */
    struct pollfd fd;
    fd.events = POLLIN;
    fd.fd = iez.get_file_descriptor();

    status = poll(&fd, 1, 5000);
    if (status == 0)
    {
      std::cerr << "poll() timed out" << std::endl;
      carry_on = false;
    }
    else if (status > 0)
    {
      std::cout << "poll event" << std::endl;
      iez.handle_inotify_event();
    }
    else
    {
      std::cerr << "ERROR: poll() failed. errno=" << errno << " " << strerror(errno) << std::endl;
      carry_on = false;
    }

  }
  return return_code;
}

void file_read_eventloop()
{
  boost::posix_time::ptime timestamp(boost::posix_time::microsec_clock::local_time());
  CBFImage cbf("blah.cbf", timestamp);
  cbf.load_data();
  CBFImage::ImageInfo_t image_info;
  cbf.get_info(image_info);

}

int main(int argc, char** argv)
{
  file_read_eventloop();
  return 0;
}
