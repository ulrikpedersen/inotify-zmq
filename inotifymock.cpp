//
// Created by Ulrik K Pedersen on 2018-12-22.
//
#ifndef __linux__

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
  return 0;
}

int inotify_rm_watch (int __fd, int __wd)
{
  return 0;
}


#endif
