
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "OSC/OSCMessage.h"
#include "OSC/SimpleWriter.h"
#include "Serial.h"
#include "UdpSocket.h"
#include "SLIPEncodedSerial.h"


static const uint8_t eot = 0300;




// reset to default turn on state
void reset(OSCMessage &msg){
}

void gotone(OSCMessage &msg){
//    printf("got a /thing/test msg \n");
}

int main(int argc, char* argv[]) {
      
    uint32_t seconds = 0;
    char osc_packet_in[256];
    //uint8_t osc_packet_in[256];
    uint8_t i = 0;
    int len = 0;


    UdpSocket udpinsock(4001);
    UdpSocket udpoutsock(4004);
 
    Serial serial;
    SLIPEncodedSerial slip;

    SimpleWriter dump;
    
    SimpleWriter serialOSCIn;


    OSCMessage msgIn;
    
    printf("cool\n");
    //the message wants an OSC address as first argument
    OSCMessage msg("/sys/renumber");
    // msg.add(888);
       

    msg.send(dump);

    printf("\n msg is %d bytes\n", dump.bufferIndex + 1);

    for (i = 0; i < dump.length; i++){
        printf("%x ", dump.buffer[i]);
    }

    // UDP send
    udpoutsock.setDestination(4000, "localhost");
    udpoutsock.writeBuffer(dump.buffer, dump.length);


    /* test slip encoding
    //slipOut.encode(dump.buffer, dump.length);
    printf("\nslip endoded: \n");
    for (i = 0; i < slipOut.length; i++){
        printf("%x ", slipOut.buffer[i]);
    }*/

    msg.setAddress("/sys/renumber");
    msg.send(dump);
    slip.sendPacket(dump.buffer, dump.length, serial);
    //slip.sendPacket(dump.buffer, dump.length, serial);
    //slip.encode(dump.buffer, dump.length);
    //serial.writeBuffer(slip.buffer, slip.length);
    //serial.writeBuffer(slip.buffer, slip.length);


/*
    msg.setAddress("/sys/renumber");
    msg.send(dump);
    slip.encode(dump.buffer, dump.length);
    serial.writeBuffer(slip.buffer, slip.length);
*/

    printf("\ndone, now gonna receive\n");


    for (;;){
        // upd recive
        len = udpinsock.readBuffer(osc_packet_in, 256, 0);
        if (len > 0){
 //           printf("received %d bytes\n", len);

            for (i = 0; i < len; i++){
                //printf("%x ", osc_packet_in[i]);
                msgIn.fill(osc_packet_in[i]);
            }
            if(!msgIn.hasError()){
             //   printf("got a message \n");
                msgIn.dispatch("/thing/test", gotone, 0);
                // send it along
                msgIn.send(dump);
                slip.sendPacket(dump.buffer, dump.length, serial);
            }
            else {
                printf("bad message");
            }
            msgIn.empty();
            //printf("\n");
        }

        if(slip.recvPacket(serial, udpoutsock)) {
/*            printf("\nslip decoded: \n");
            for (i = 0; i < slip.length; i++){
                printf("%x ", slip.buffer[i]);
            }*/
            udpoutsock.writeBuffer(slip.buffer, slip.length);

        }
        usleep(1000);
    } // for;;
}
