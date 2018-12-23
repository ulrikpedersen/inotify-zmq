//
// Created by Ulrik K Pedersen on 2018-12-22.
//
#include <iostream>
#include <string>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

#include "inotifymock.h"

int inotify_init (void)
{
  std::string fname = "blahblah.h5";
  size_t fname_str_size = fname.size() + 1;

  struct inotify_event event;
  event.cookie = 321;
  event.len = 128-16;
  event.mask = IN_MOVED_TO;
  event.wd = 54;
  event.name[fname_str_size - 1] = '\0';
  strncpy(event.name, fname.c_str(), fname_str_size);

  int fd = open("mockinotifyfile-deleteme", O_CREAT|O_WRONLY);
  write(fd, &event, sizeof(event));
  fsync(fd);
  close(fd);

  fd = open("mockinotifyfile-deleteme", O_RDONLY);

  return fd;
}

int inotify_add_watch (int __fd, const char *__name, unsigned int __mask)
{
  int fd = open("mockinotifyfile-deleteme", O_RDONLY);
  if (fd != __fd) std::cerr << "ERROR: inotify_add_watch() didn't open the same file descriptor" << std::endl;

  return 0;
}

int inotify_rm_watch (int __fd, int __wd)
{
  close(__fd);
  return 0;
}

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
