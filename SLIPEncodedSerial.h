
#ifndef SLIPEncodedSerial_h
#define SLIPEncodedSerial_h

#include <stdint.h> 
#include "Serial.h"
#include "UdpSocket.h"

#define RX_BUF_SIZE 256
#define WAITING 1
#define RECEIVING 2


class SLIPEncodedSerial
{

private:

public:
	
	SLIPEncodedSerial();
    
    uint8_t rstate;

    uint8_t buffer[1024];
    uint32_t bufferIndex;
    uint32_t length;


    uint8_t serialIn[256]; // for reading from serial port

    uint8_t rxBuf[RX_BUF_SIZE + 2];  // circular buffer for incoming serial
    uint32_t rxBufReadIndex;
    uint32_t rxBufWriteIndex;
   
    uint8_t rxPacket[256];
    uint32_t rxPacketIndex;

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
