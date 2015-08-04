#include "SLIPEncodedSerial.h"

/*
 CONSTRUCTOR
 */
SLIPEncodedSerial::SLIPEncodedSerial()
{


    rstate = WAITING;

    rxBufReadIndex = 0;
    rxBufWriteIndex = 0;
    rxPacketIndex = 0;


}

static const uint8_t eot = 0300;
static const uint8_t slipesc = 0333;
static const uint8_t slipescend = 0334;
static const uint8_t slipescesc = 0335;


int SLIPEncodedSerial::sendPacket(const uint8_t *buf, uint32_t len, Serial &s)
{
    encode(buf, len);
    s.writeBuffer(buffer, length);
}

//int SLIPEncodedSerial::recvPacket(uint8_t * buf, uint32_t len)
int SLIPEncodedSerial::recvPacket(Serial &s)
{
    int i, len;
    
    // fill up recv buffer from serial port
    len = s.readBuffer(serialIn, 256);
    if (len == -1) {
        //    printf("Error reading from serial port\n");
    }
    else if (len == 0) {
        //printf("No more data\n");
    }
    else {
        for (i = 0;  i < len; i++) {
            rxBuf[rxBufWriteIndex++] = serialIn[i];
            if (rxBufWriteIndex > RX_BUF_SIZE) rxBufWriteIndex = 0;
        }
    }
   
    // fill rx buf
    /*for (i = 0;  i < len; i++) {
        rxBuf[rxBufWriteIndex++] = buf[i];
        if (rxBufWriteIndex > RX_BUF_SIZE) rxBufWriteIndex = 0;
    }*/

    // process rx buffer, this might return before the whole thing
    // is proccessed,  but we'll just get it next time
    while (rxBufReadIndex != rxBufWriteIndex) {
       
       uint8_t tmp8 = rxBuf[rxBufReadIndex++];
        if (rxBufReadIndex > RX_BUF_SIZE) rxBufReadIndex = 0;
    
      //  uint8_t tmp8 = serialIn[i];
        
        if (rstate == WAITING) {
            if (tmp8 == eot) rstate = WAITING; // just keep waiting for something afer EOT
            else {
                rxPacketIndex = 0;
                rxPacket[rxPacketIndex++] = tmp8;
                rstate = RECEIVING;
            }
        } // waiting
        else if (rstate == RECEIVING){
            if (tmp8 == eot) {  //TODO:  exit if message len > max
                rstate = WAITING;
                decode(rxPacket, rxPacketIndex);
                return 1;
            }
            else {
                rxPacket[rxPacketIndex++] = tmp8;
                rstate = RECEIVING;
            }
        } //receiving
    } // gettin bytes

    return 0;
}
 
//encode SLIP
void SLIPEncodedSerial::encode(uint8_t b){
    if(b == eot){
        buffer[bufferIndex++] = slipesc;
        buffer[bufferIndex++] = slipescend;
    } else if(b==slipesc) {
        buffer[bufferIndex++] = slipesc;
        buffer[bufferIndex++] = slipescesc;
   } else {
        buffer[bufferIndex++] = b;
    }
}

void SLIPEncodedSerial::encode(const uint8_t *buf, int size) 
{  
    beginPacket();
    while(size--) encode(*buf++); 
    endPacket();
}

void SLIPEncodedSerial::decode(const uint8_t *buf, int size)
{
    int i;
    bufferIndex = 0;
    i = 0;

    while (i < size) {
        if (buf[i] == slipesc) {  // TODO error out here if slipescend or slipescesc doesn't follow slipesc
            i++;
            if (buf[i] == slipescend) buffer[bufferIndex++] = eot;
            if (buf[i] == slipescesc) buffer[bufferIndex++] = slipesc;
            i++;
        }
        else {
            buffer[bufferIndex++] = buf[i];
            i++;
        }
    }
    length = bufferIndex;
}


//SLIP specific method which begins a transmitted packet
void SLIPEncodedSerial::beginPacket() {
    bufferIndex = 0;
    buffer[bufferIndex] = eot;
    bufferIndex++;
}

//signify the end of the packet with an EOT
void SLIPEncodedSerial::endPacket(){
    buffer[bufferIndex] = eot;
    length = bufferIndex + 1;
}


