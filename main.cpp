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


int main()
{
  int return_code = 0;
  int status = 0;
  std::cout << "Hello, World!" << std::endl;
  zmq::context_t context (1);

  InotifyEventZmq iez(context);

  const char * pathname = ".";
  iez.set_filename_pattern_regex(".*h5");
  iez.watch_dir(pathname);

  bool carry_on = true;
  while(carry_on)
  {                                  /* Read events forever */
    struct pollfd fd;
    fd.events = POLLIN;
    fd.fd = iez.get_file_descriptor();

    status = poll(&fd, 1, 1000);
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

