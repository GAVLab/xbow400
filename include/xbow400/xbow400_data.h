#ifndef XBOW400_DATA_H
#define XBOW400_DATA_H

namespace xbow400 {
  struct IMU400CC_100 {
    // IMU Temp
    double temp;
    // Gyros
    double roll_rate;
    double pitch_rate;
    double yaw_rate;
    // Accelerometers
    double x_accel;
    double y_accel;
    double z_accel;
    // Time (counts 65,535 down to 0)
    size_t time;
  };
}

#endif
