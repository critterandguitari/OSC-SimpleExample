#include "SLIPEncoderDecoder.h"

/*
 CONSTRUCTOR
 */
SLIPEncoderDecoder::SLIPEncoderDecoder(){
}

static const uint8_t eot = 0300;
static const uint8_t slipesc = 0333;
static const uint8_t slipescend = 0334;
static const uint8_t slipescesc = 0335;

//encode SLIP
void SLIPEncoderDecoder::encode(uint8_t b){
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

void SLIPEncoderDecoder::encode(const uint8_t *buf, int size) 
{  
    beginPacket();
    while(size--) encode(*buf++); 
    endPacket();
}

void SLIPEncoderDecoder::decode(const uint8_t *buf, int size)
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
void SLIPEncoderDecoder::beginPacket() {
    bufferIndex = 0;
    buffer[bufferIndex] = eot;
    bufferIndex++;
}

//signify the end of the packet with an EOT
void SLIPEncoderDecoder::endPacket(){
    buffer[bufferIndex] = eot;
    length = bufferIndex + 1;
}


