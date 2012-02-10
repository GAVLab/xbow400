#include "xbow400/xbow400.h"

using xbow400::Xbow400;

void onNewData(const xbow400::Xbow400_Data &data) {
  // do soemthing
  
}

int main(int argc, char **argv) {
  Xbow400 my_xbow;

  my_xbow.connect("/dev/ttyUSB0");

  my_xbow.setDataCallback(onNewData);

  while (true) {
    my_xbow.readOnce();
  }

  return 0;
}
