//
// Created by up45 on 01/01/19.
//

#include "FilePushZmq.h"

CBFImage::CBFImage(const std::string &fname, const boost::posix_time::ptime &timestamp) :
fname(fname), timestamp(timestamp)
{

}

CBFImage::~CBFImage() {

}

void CBFImage::load_data()
{

}

void CBFImage::get_info(CBFImage::ImageInfo_t &image_info) {

}

size_t CBFImage::get_image_data_size() {
  return 0;
}
