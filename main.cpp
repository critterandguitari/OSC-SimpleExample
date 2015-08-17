
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

#include "UdpSocket.h"

//SLIPEncodedSerial SLIPSerial;

SimpleWriter dump;

// reset to default turn on state
void reset(OSCMessage &msg){
}

void renumber(OSCMessage &msg){
}

int main(int argc, char* argv[]) {
      
    uint32_t seconds = 0;
    char osc_packet_in[256];
    //uint8_t osc_packet_in[256];
    uint8_t i = 0;
    uint32_t len = 0;


    UdpSocket udpinsock(4001);
    UdpSocket udpoutsock(4004);
    
    //udp_send_init();
    //pd_receive_init();

    printf("cool\n");
    //the message wants an OSC address as first argument
    OSCMessage msg("/thing/test");
    msg.add(888);
        
    dump.start();  
    msg.send(dump);
    dump.end();

    printf("\n msg is %d bytes\n", dump.bufferIndex + 1);

    for (i = 0; i < dump.length; i++){
        printf("%x ", dump.buffer[i]);
    }

    //udp_send_buf(dump.buffer, dump.length);
    //pd_udp_send_buf
    udpoutsock.setDestination(4000, "localhost");
    udpoutsock.writeBuffer(dump.buffer, dump.length);

    printf("\ndone, now gonna receive\n");

    for (;;){
      //  len = pd_receive_poll(osc_packet_in);
        //len = udpread(osc_packet_in);
        len = udpinsock.readBuffer(osc_packet_in, 256, 0);
        if (len > 0)
            printf("received %d bytes\n", len);
    }

}
