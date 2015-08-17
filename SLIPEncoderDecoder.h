
#ifndef SLIPEncoderDecoder_h
#define SLIPEncoderDecoder_h

#include <stdint.h> 


class SLIPEncoderDecoder
{
	
public:
	
	SLIPEncoderDecoder();

    uint8_t buffer[1024];
    uint32_t bufferIndex;
    uint32_t length;
    
    //SLIP specific method which begins a transmitted packet
	void beginPacket();
	
	//SLIP specific method which ends a transmittedpacket
	void endPacket();
	
	void encode(uint8_t b);
    // void write(const uint8_t *buffer, size_t size);
    void encode(const uint8_t *buffer, int size);

};


#endif
