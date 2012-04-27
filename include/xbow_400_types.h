#include <sstream>

/****  Defines ****/

// XBOW Modes


#define NORMAL_HEADER 0xFF
#define NORMAL_HEADER_SIZE 1

#define EXTENDED_HEADER 0xAA55
#define EXTENDED_HEADER_SIZE 2
#define EXTENDED_HEADER_FIRST_BYTE 0xAA

#define EXTENDED_HEADER_SECOND_BYTE 0x55

#define HDX_RAW_PACKET_SIZE 18
#define HDX_FAST_RAW_PACKET_SIZE 16
#define HDX_COOKED_PACKET_SIZE 18
#define HDX_ANGLE_PACKET_SIZE 22

#define AHRS_RAW_PACKET_SIZE 24
#define AHRS_FAST_RAW_PACKET_SIZE 22
#define AHRS_COOKED_PACKET_SIZE 24
#define AHRS_ANGLE_PACKET_SIZE 30

#define NEW_ANGLE_PACKET_SIZE 26

#define REMOTEMAG_PACKET_SIZE 20

#define VOLT_RANGE 5.0
#define MAG_VOLT_RANGE 5.0
#define TWO_EXP_TWELVE 4095
#define TWO_EXP_FIFTEEN 32767
#define TWO_EXP_SIXTEEN 65535

/**** Data Structurs ****/

struct IMU400CC_100 {
    //IMU
    double IMUtemp;
    //gyros
    double yaw;
    double roll;
    double pitch;
    // accelerometers
    double x_accel;
    double y_accel;
    double z_accel;
    
};

// // template <typename T> // note, might as well take p as const-reference
// std::ostream& operator<<(std::ostream &out, const IMU400CC_100& imu400cc_100) {
//     out << "IMU400CC_100 Packet:";
//     out << " IMU Temperature: " << imu400cc_100.IMUtemp;
//     out << " Gyro Yaw: " << imu400cc_100.yaw;
//     out << " Gyro Roll: " << imu400cc_100.roll;
//     out << " Gyro Pitch: " << imu400cc_100.pitch;
//     out << " Accel X: " << imu400cc_100.x_accel;
//     out << " Accel Y: " << imu400cc_100.y_accel;
//     out << " Accel Z: " << imu400cc_100.z_accel;
//     return out;
// }

class ConnectionFailedException : public std::exception {
    const char * e_what;
public:
    ConnectionFailedException(const char * e_what) {this->e_what = e_what;}
    
    virtual const char* what() const throw() {
        std::stringstream ss;
        ss << "Error connecting to Xbow400: " << this->e_what;
        return ss.str().c_str();
    }
};

class ReadFailedException : public std::exception {
    const char * e_what;
public:
    ReadFailedException(const char * e_what) {this->e_what = e_what;}
    
    virtual const char* what() const throw() {
        std::stringstream ss;
        ss << "Error reading data from Xbow400: " << this->e_what;
        return ss.str().c_str();
    }
};