
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
#include "Serial.h"
#include "UdpSocket.h"
#include "SLIPEncoderDecoder.h"

//SLIPEncodedSerial SLIPSerial;

SimpleWriter dump;

// reset to default turn on state
void reset(OSCMessage &msg){
}

void gotone(OSCMessage &msg){
    printf("got a /thing/test msg \n");
}

int main(int argc, char* argv[]) {
      
    uint32_t seconds = 0;
    char osc_packet_in[256];
    //uint8_t osc_packet_in[256];
    uint8_t i = 0;
    uint32_t len = 0;


    UdpSocket udpinsock(4001);
    UdpSocket udpoutsock(4004);
 
    Serial serial;
    SLIPEncoderDecoder slipOut;

    //udp_send_init();
    //pd_receive_init();

    printf("cool\n");
    //the message wants an OSC address as first argument
    OSCMessage msg("/sys/renumber");
   // msg.add(888);
       

    OSCMessage msgIn;

    msg.send(dump);

    printf("\n msg is %d bytes\n", dump.bufferIndex + 1);

    for (i = 0; i < dump.length; i++){
        printf("%x ", dump.buffer[i]);
    }

    //udpoutsock.setDestination(4000, "localhost");
    //udpoutsock.writeBuffer(dump.buffer, dump.length);

    slipOut.encode(dump.buffer, dump.length);

    printf("\nslip endoded: \n");
    for (i = 0; i < slipOut.length; i++){
        printf("%x ", slipOut.buffer[i]);
    }


    serial.writeBuffer(slipOut.buffer, slipOut.length);
           //writeBuffer(void *buffer, long len);

    printf("\ndone, now gonna receive\n");

    for (;;){
      //  len = pd_receive_poll(osc_packet_in);
        //len = udpread(osc_packet_in);
        len = udpinsock.readBuffer(osc_packet_in, 256, 0);
        if (len > 0){
            printf("received %d bytes\n", len);

            for (i = 0; i < len; i++){
                printf("%x ", osc_packet_in[i]);
                msgIn.fill(osc_packet_in[i]);
            }
            if(!msgIn.hasError()){
                printf("got a message \n");
                
                msgIn.dispatch("/thing/test", gotone, 0);
            }
            else {
                printf("bad message");
            }
            msgIn.empty();
            printf("\n");
        }
    }
}
