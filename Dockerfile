
# build image:
#     docker build --target build -t inotify-zmq .
# run image:
#     docker run -it --rm --entrypoint /bin/bash -v `pwd`:/src/inotify-zmq inotify-zmq

FROM centos:7 as build

# Set the working directory to /src
WORKDIR /src

RUN yum -y update && yum -y clean all &&\
    yum groupinstall -y 'Development Tools' &&\
    yum install -y boost boost-devel cmake log4cxx-devel epel-release &&\
    yum install -y zeromq3-devel python2-pip python-zmq &&\
    yum -y clean all && rm -rf /var/cache/yum &&\
    cd / # && rm -rf /src/build* /src/*.tar*

WORKDIR /src/

