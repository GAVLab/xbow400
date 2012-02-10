#ifndef XBOW400_H
#define XBOW400_H

#include <string>

#include <serial/serial.h>

#include "xbow400_data.h"

typedef void(*DataCallback)(const xbow400::Xbow400_Data&);

namespace xbow400 {
  class Xbow400 {
  public:
    Xbow400 (std::string port_name = "");
    ~Xbow400 ();

    void connect (std::string port_name = "");
    void disconnect ();

    void setDataCallback (DataCallback data_cb);

    void readOnce ();

  private:
    serial::Serial serial_port_;
    std::string port_name_;
    bool is_connected_;

    DataCallback data_cb_;
  };
}

#endif
