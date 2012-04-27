#ifndef XBOW_400_H
#define XBOW_400_H

#include <iostream>
#include <string.h>

#include "boost/thread.hpp"

#include "serial.h"

#include "xbow_400_types.h"

static const int baudrate = 38400;
static const long timeout = 1000;
static const int packet_size = 18;
static const std::string ping_cmd = "R";
static const std::string ping_response = "H";
static const int number_of_attempts = 10;
static const std::string polled_mode = "P";
static const std::string continuous_mode = "C";
static const std::string voltage_mode = "r";
static const std::string voltage_mode_response = "R";
static const std::string inertial_mode = "c";
static const std::string inertial_mode_response = "C";
static const std::string version_query = "v";
static const std::string serial_number_query = "S";
static const std::string request_data_cmd = "G";

static const double INERTIAL_GYRO_CONST = (100.0*1.5)/TWO_EXP_FIFTEEN;
static const double INERTIAL_ACCEL_CONST = (2.0*1.5)/TWO_EXP_FIFTEEN;

class Xbow_400 {
public:
    Xbow_400(std::string port);
    ~Xbow_400();
    
    void connect();
    void reconnect();
    void disconnect();
    
    IMU400CC_100 getData();
    
    void getData(IMU400CC_100 &packet);
    
    std::string getPort() const;
    
    void setPort(std::string port);
    
    std::string getDMUVersion() const;
    
    unsigned int getSerialNumber() const;
    
    void setDataCallback(void (*callback)(IMU400CC_100 &packet));
    
    void startContinuousRead();
    void stopContinuousRead();
private:
    bool check_checksum(const char *packet, int packet_size, char checksum);
    void parsePacket(char* buffer, IMU400CC_100 &packet, int offset = 1);
    void _readContinuously();
    
    std::string port;
    serial::Serial serial_port;
    bool connected;
    
    std::string DMU_version;
    unsigned int serial_number;
    
    void (*callback)(IMU400CC_100 &packet);
    boost::thread contiuous_read_thread;
    bool continuous;
};

#endif