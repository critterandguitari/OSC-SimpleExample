
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "OSC/OSCMessage.h"
#include "OSC/SimpleWriter.h"
#include "Serial.h"
#include "UdpSocket.h"
#include "SLIPEncodedSerial.h"


static const uint8_t eot = 0300;

// example handler
void gotone(OSCMessage &msg){
//    printf("got a /thing/test msg \n");
}

int main(int argc, char* argv[]) {
      
    uint32_t seconds = 0;
    char udpPacketIn[256];
    //uint8_t osc_packet_in[256];
    uint8_t i = 0;
    int len = 0;


    UdpSocket udpSock(4001);
    udpSock.setDestination(4000, "localhost");
 
    Serial serial;
    SLIPEncodedSerial slip;

    SimpleWriter dump;

    OSCMessage msgIn;
    
    printf("cool\n");

    // make an osc object
    // the message wants an OSC address as first argument
    OSCMessage msg("/sys/renumber");
    // msg.add(888);
       
    // this just dumps it into the simple writer dump
    msg.send(dump);

    printf("\n msg is %d bytes\n", dump.length);

    // print it out
    for (i = 0; i < dump.length; i++){
        printf("%x ", dump.buffer[i]);
    }

    // UDP send
    udpSock.writeBuffer(dump.buffer, dump.length);

    // serial send
    slip.sendMessage(dump.buffer, dump.length, serial);

    printf("\ndone, now gonna receive\n");


    // full udp -> serial -> serial -> udp
    for (;;){
        // receive udp, send to serial
        len = udpSock.readBuffer(udpPacketIn, 256, 0);
        if (len > 0){
            for (i = 0; i < len; i++){
                msgIn.fill(udpPacketIn[i]);
            }
            if(!msgIn.hasError()){
                msgIn.dispatch("/thing/test", gotone, 0);
                // send it along
                msgIn.send(dump);
                slip.sendMessage(dump.buffer, dump.length, serial);
            }
            else {
                printf("bad message");
            }
            msgIn.empty();
        }   

        // receive serial, send udp
        if(slip.recvMessage(serial)) {
            udpSock.writeBuffer(slip.decodedBuf, slip.decodedLength);
        }

        // sleep for 1ms
        usleep(1000);
    } // for;;
}
