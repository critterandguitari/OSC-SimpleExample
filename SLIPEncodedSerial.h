
#ifndef SLIPEncodedSerial_h
#define SLIPEncodedSerial_h

#include <stdint.h> 
#include "Serial.h"
#include "UdpSocket.h"

#define MAX_MSG_SIZE 256 // the maximum un encoded size.  the max encoded size will be this * 2 for slip overhead
#define SERIAL_READ_SIZE 64
#define WAITING 1
#define RECEIVING 2

class SLIPEncodedSerial
{

private:

public:
	
	SLIPEncodedSerial();
    
    uint8_t rstate;

    // incoming encoded message
    uint8_t encodedInBuf[MAX_MSG_SIZE * 2];    // the encoded can be up to 2 * longer
    uint32_t encodedInBufIndex;
    uint32_t encodedInLength;

    // incoming message decoded
    uint8_t decodedInBuf[MAX_MSG_SIZE];
    uint32_t decodedInBufIndex;
    uint32_t decodedInLength;

    // outgoing encoded message
    uint8_t encodedOutBuf[MAX_MSG_SIZE * 2];    // the encoded can be up to 2 * longer
    uint32_t encodedOutBufIndex;
    uint32_t encodedOutLength;

    uint8_t serialIn[SERIAL_READ_SIZE]; // for reading from serial port, read in 64 byte chunks
    uint8_t rxBuf[SERIAL_READ_SIZE * 2];  // circular buffer for incoming serial, should be 2 times serail read size
    uint32_t rxBufHeadx;
    uint32_t rxBufTail;

    //SLIP specific method which begins a transmitted packet
	void beginPacket();
	
	//SLIP specific method which ends a transmittedpacket
	void endPacket();
   
    void encode(const uint8_t *buf, int size);
	void encode(uint8_t b);
    
    void decode(const uint8_t *buf, int size);

    int sendPacket(const uint8_t *buf, uint32_t len, Serial &s);

    int recvPacket(Serial &s, UdpSocket &udpoutsock);
    
};


#endif
