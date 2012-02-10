#include "xbow400/xbow400.h"

using namespace xbow400;

Xbow400::Xbow400 (std::string port_name) {
  this->is_connected_ = false;
  if (!port_name.empty()) {
    this->connect(port_name);
  }
  this->data_cb_ = NULL;
}

Xbow400::~Xbow400 () {
  this->disconnect();
}

void
Xbow400::connect (std::string port_name) {
  if (!port_name.empty()) {
    this->port_name_ = port_name;
  }

  this->serial_port_.setPort(this->port_name_);
  this->serial_port_.setBaudrate(38400);
  this->serial_port_.setTimeout(250);

  this->serial_port_.open();
  this->is_connected_ = true;
}

void
Xbow400::disconnect () {
  if (this->serial_port_.isOpen()) {
    this->serial_port_.close();
  }
}

void
Xbow400::setDataCallback (DataCallback data_cb) {
  this->data_cb_ = data_cb;
}

void
Xbow400::readOnce () {
  // Get some data
  std::string temp = this->serial_port_.read(16);
  // Put it in the structure
  Xbow400_Data data;
  data.temp = (int)(temp[0]+temp[1] >> 8);
  // Return
  if (this->data_cb_ != NULL) {
    this->data_cb(data);
  }
}
