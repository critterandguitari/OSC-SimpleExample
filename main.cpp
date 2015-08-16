
#include <stdint.h>
#include <stdio.h>



extern "C" {
//#include "uart.h"
//#include "stm32f0xx.h"
//#include "Timer.h"
//#include "BlinkLed.h"
}

#include "OSC/OSCMessage.h"
#include "OSC/SimpleWriter.h"
#include "udp_send.h"

//SLIPEncodedSerial SLIPSerial;

SimpleWriter dump;

// reset to default turn on state
void reset(OSCMessage &msg){
}

void renumber(OSCMessage &msg){
}

int main(int argc, char* argv[]) {
      
    uint32_t seconds = 0;
    uint8_t osc_message[256];
    uint8_t i = 0;

    udp_send_init();

    printf("cool\n");
    //the message wants an OSC address as first argument
    OSCMessage msg("/thing/test");
    msg.add(909);
        
    dump.start();  
    msg.send(dump);
    dump.end();

    printf("\n msg is %d bytes\n", dump.bufferIndex + 1);

    for (i = 0; i < dump.length; i++){
        printf("%x ", dump.buffer[i]);
    }

    udp_send_buf(dump.buffer, dump.length);
    //pd_udp_send_buf
    printf("\ndone\n");
}
