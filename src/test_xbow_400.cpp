#include "xbow_400.h"

void callback(IMU400CC_100 &packet) {
    std::cout << "IMU400CC_100 Packet:" << std::endl;
    std::cout << " IMU Temperature: " << packet.IMUtemp << std::endl;
    std::cout << " Gyro Yaw: " << packet.yaw << std::endl;
    std::cout << " Gyro Roll: " << packet.roll << std::endl;
    std::cout << " Gyro Pitch: " << packet.pitch << std::endl;
    std::cout << " Accel X: " << packet.x_accel << std::endl;
    std::cout << " Accel Y: " << packet.y_accel << std::endl;
    std::cout << " Accel Z: " << packet.z_accel << std::endl;
    std::cout << std::endl;
}

int main(void) {
    Xbow_400 xbow("/dev/tty.usbserial");
    xbow.connect();
    std::cout << "Connected to Xbow400 with DMU version: " << xbow.getDMUVersion() << " and Serial Number: " << xbow.getSerialNumber() << std::endl;
    
    xbow.setDataCallback(callback);
    xbow.startContinuousRead();
    while(true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(33));
    }
    xbow.stopContinuousRead();
}