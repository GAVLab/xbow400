#include "xbow_400.h"

inline void default_callback(IMU400CC_100 &packet) {
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

Xbow_400::Xbow_400(std::string port) : connected(false), serial_number(0), continuous(false) {
    this->port = port;
    callback = default_callback;
}

Xbow_400::~Xbow_400() {
    this->disconnect();
}

inline unsigned long swap_32bit_endianess(unsigned long ul) {
    return (unsigned long)(((ul & 0xFF000000) >> 24) |
                           ((ul & 0x00FF0000) >>  8) |
                           ((ul & 0x0000FF00) <<  8) |
                           ((ul & 0x000000FF) << 24));
}

void Xbow_400::connect() {
    try {
        // Configure and open the serial port
        this->serial_port.setPort(this->port);
        this->serial_port.setBaudrate(baudrate);
        this->serial_port.setTimeoutMilliseconds(timeout);
        this->serial_port.open();
    } catch(std::exception &e) {
        throw(ConnectionFailedException(e.what()));
    }
    
    // Set the xbow to polled mode
    if(this->serial_port.write(std::string("G")) != 1)
        throw(ConnectionFailedException("Failed to put in polled mode."));
    
    // Ping the xbow
    int count = 0;
    while(!this->connected && count != number_of_attempts) {
        this->serial_port.write(ping_cmd);
        if(this->serial_port.read(1000) == ping_response) {
            this->connected = true;
        } else {
            if(this->serial_port.write(std::string("P")) != 1)
                throw(ConnectionFailedException("Failed to put in polled mode."));
        }
        count += 1;
    }
    
    // Did we connect?
    if(!this->connected)
        throw(ConnectionFailedException("Failed to contact the xbow, is it connected and on?"));
    
    // Get DMU version
    this->serial_port.write(version_query);
    char *buffer = new char[32];
    int bytes_read = this->serial_port.read(buffer, 32);
    this->DMU_version = std::string(buffer);
    std::string checksum = this->DMU_version.substr(this->DMU_version.length()-1);
    this->DMU_version = DMU_version.substr(1, this->DMU_version.length()-2);
    if(!this->check_checksum(this->DMU_version.c_str(), this->DMU_version.length(), checksum[0]))
        throw(ConnectionFailedException("Failed to read the DMU Version, invalid checksum."));
    
    // Get Serial
    this->serial_port.write(serial_number_query);
    buffer = new char[32];
    bytes_read = this->serial_port.read(buffer, 1);
    if(buffer[0] != char(0xFF))
        throw(ConnectionFailedException("Failed to read the serial number."));
    bytes_read = this->serial_port.read(buffer, 4);
    std::string chksum = this->serial_port.read(1); // Read off checksum
    if(!check_checksum(buffer, 4, chksum.c_str()[0]))
        throw(ConnectionFailedException("Failed to read the serial number, invalid checksum."));
    memcpy(&this->serial_number, buffer, 4);
    this->serial_number = swap_32bit_endianess(this->serial_number);
    
    // Set the mode to inertial mode
    // if(this->serial_port.write(voltage_mode) != 1 || this->serial_port.read(1) != voltage_mode_response)
    if(this->serial_port.write(inertial_mode) != 1 || this->serial_port.read(1) != inertial_mode_response)
        throw(ConnectionFailedException("Failed to put in inertial mode."));
    
    delete[] buffer;
}

void Xbow_400::disconnect() {
    this->connected = false;
    this->serial_port.close();
}

void Xbow_400::reconnect() {
    this->disconnect();
    this->connect();
}

IMU400CC_100 Xbow_400::getData() {
    IMU400CC_100 result;
    this->getData(result);
    return result;
}

inline void printHex(char * data, int length) {
    for(int i = 0; i < length; ++i) {
        printf("0x%.2X ", (unsigned)(unsigned char)data[i]);
    }
    printf("\n");
}

void Xbow_400::getData(IMU400CC_100 &packet) {
    this->serial_port.write(request_data_cmd);
    char *buffer = new char[32];
    int bytes_read = this->serial_port.read(buffer, packet_size);
    if(bytes_read != 18)
        throw(ReadFailedException("Timeout occured while reading."));
    
    packet = IMU400CC_100();
    
    if(buffer[0] != char(0xFF))
        throw(ReadFailedException("Invalid start byte."));
    
    this->parsePacket(buffer, packet);
    
    delete[] buffer;
}

std::string Xbow_400::getPort() const {
    return this->port;
}

void Xbow_400::setPort(std::string port) {
    this->port = port;
}

std::string Xbow_400::getDMUVersion() const {
    if(this->connected)
        return this->DMU_version;
    else
        return ""; // TODO should throw exception
}

unsigned int Xbow_400::getSerialNumber() const {
    if(this->connected)
        return this->serial_number;
    else
        return 0; // TODO should throw exception
}

void Xbow_400::setDataCallback(void (*callback)(IMU400CC_100 &packet)) {
    this->callback = callback;
}

void Xbow_400::startContinuousRead() {
    this->contiuous_read_thread = boost::thread(&Xbow_400::_readContinuously, this);
}

void Xbow_400::stopContinuousRead() {
    this->continuous = false;
    this->contiuous_read_thread.join();
}

bool Xbow_400::check_checksum(const char *packet, int packet_size, char checksum) {
    char packet_sum = 0;
    for(int i = 0; i != packet_size; ++i)
        packet_sum += packet[i];
    return checksum == packet_sum;
}

void Xbow_400::parsePacket(char* buffer, IMU400CC_100 &packet, int offset) {
    switch(1) { // TODO make the mode optional
        case 0: // Voltage Mode
            packet.roll=((short)((buffer[0+offset]<<8)+buffer[1+offset]))*VOLT_RANGE/TWO_EXP_SIXTEEN;
            packet.pitch=((short)((buffer[2+offset]<<8)+buffer[3+offset]))*VOLT_RANGE/TWO_EXP_SIXTEEN;
            packet.yaw=((short)((buffer[4+offset]<<8)+buffer[5+offset]))*VOLT_RANGE/TWO_EXP_SIXTEEN;
            packet.x_accel=((short)((buffer[6+offset]<<8)+buffer[7+offset]))*VOLT_RANGE/TWO_EXP_SIXTEEN;
            packet.y_accel=((short)((buffer[8+offset]<<8)+buffer[9+offset]))*VOLT_RANGE/TWO_EXP_SIXTEEN;
            packet.z_accel=((short)((buffer[10+offset]<<8)+buffer[11+offset]))*VOLT_RANGE/TWO_EXP_SIXTEEN;
            packet.IMUtemp=((((buffer[12+offset]<<8)+buffer[13+offset])*5.0/4096.0)-1.375)*44.44;
            break;
        case 1: // Inertial Mode
            packet.roll = ((((short)buffer[0+offset])<<8) + ((short)buffer[1+offset])) * INERTIAL_GYRO_CONST;
            packet.pitch=((short)((buffer[2+offset]<<8)+buffer[3+offset]))*INERTIAL_GYRO_CONST;
            packet.yaw=((short)((buffer[4+offset]<<8)+buffer[5+offset]))*INERTIAL_GYRO_CONST;
            packet.x_accel=((short)((buffer[6+offset]<<8)+buffer[7+offset]))*INERTIAL_ACCEL_CONST;
            packet.y_accel=((short)((buffer[8+offset]<<8)+buffer[9+offset]))*INERTIAL_ACCEL_CONST;
            packet.z_accel=((short)((buffer[10+offset]<<8)+buffer[11+offset]))*INERTIAL_ACCEL_CONST;
            packet.IMUtemp=((((buffer[12+offset]<<8)+buffer[13+offset])*5.0/4096.0)-1.375)*44.44;
            break;
    }
}

void Xbow_400::_readContinuously() {
    // Set the device in Continuous mode
    if(this->serial_port.write(continuous_mode) != 1)
        throw(ReadFailedException("Failed to put in continuous mode."));
    
    this->continuous = true;
    
    char *buffer = new char[19];
    while(this->continuous) {
        int bytes_read = this->serial_port.read(buffer, 1);
        if(bytes_read != 1 || buffer[0] != char(0xFF)) {
            this->continuous = false;
            this->serial_port.write(std::string("G"));
            throw(ReadFailedException("Failed to read in continuous mode, Failed to get a valid start byte."));
        }
        bytes_read = this->serial_port.read(buffer, 16);
        char checksum_buffer[2];
        int checksum_bytes_read = this->serial_port.read(checksum_buffer,1);
        if(bytes_read != 16 || checksum_bytes_read != 1) {
            this->continuous = false;
            this->serial_port.write(std::string("G"));
            throw(ReadFailedException("Failed to read in continuous mode, timeout reading data."));
        }
        if(!this->check_checksum(buffer, 16, checksum_buffer[0])) {
                    // this->continuous = false;
                    // this->serial_port.write(std::string("G"));
                    // throw(ReadFailedException("Failed to read in continuous mode, bad checksum."));
            std::cerr << "Failed to read in continuous mode due to bad checksum, skipping packet." << std::endl;
            continue;
        }
        IMU400CC_100 packet;
        this->parsePacket(buffer, packet, 0);
        this->callback(packet);
        delete[] buffer;
        buffer = new char[19];
    }
    delete[] buffer;
    this->serial_port.write(std::string("G"));
    
    // Set the xbow to polled mode
    if(this->serial_port.write(polled_mode) != 1)
        throw(ConnectionFailedException("Failed to put in polled mode."));
}






