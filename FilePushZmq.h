//
// Created by up45 on 01/01/19.
//

#ifndef FILETOZMQ_FILEPUSHZMQ_H
#define FILETOZMQ_FILEPUSHZMQ_H

#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>

#include "zmq.hpp"


class CBFImage {
public:

  struct ImageInfo_t {
    unsigned int xdim;
    unsigned int ydim;
    unsigned int bytes_per_pixel;
    size_t data_buffer_size;
  };

  explicit CBFImage(const std::string& fname, const boost::posix_time::ptime &timestamp);
  ~CBFImage();
  void load_data();
  boost::shared_ptr<void> get_image_data() { return this->image_data; }
  void get_info(CBFImage::ImageInfo_t &image_info);
  size_t get_image_data_size();

private:
  std::string header_data;
  boost::shared_ptr<void> image_data;
  const std::string fname;
  const boost::posix_time::ptime timestamp;
};


class FilePushZmq {
public:
  explicit FilePushZmq(zmq::context_t& context);
  ~FilePushZmq(){};
  void run();


private:
  zmq::socket_t input_socket;
  zmq::socket_t output_socket;

};


#endif //FILETOZMQ_FILEPUSHZMQ_H
