
#include <stdint.h>
#include <stdio.h>

#include "OSC/OSCMessage.h"
#include "OSC/SimpleWriter.h"
#include "Serial.h"
#include "UdpSocket.h"
#include "SLIPEncoderDecoder.h"


// message io stuff
#define WAITING 1
#define RECEIVING 2

uint8_t rstate = WAITING;
static const uint8_t eot = 0300;




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
    SLIPEncoderDecoder slipIn;

    SimpleWriter dump;
    
    SimpleWriter serialOSCIn;

    //udp_send_init();
    //pd_receive_init();

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

    udpoutsock.setDestination(4000, "localhost");
    //udpoutsock.writeBuffer(dump.buffer, dump.length);

    slipOut.encode(dump.buffer, dump.length);

    printf("\nslip endoded: \n");
    for (i = 0; i < slipOut.length; i++){
        printf("%x ", slipOut.buffer[i]);
    }


    //serial.writeBuffer(slipOut.buffer, slipOut.length);
           //writeBuffer(void *buffer, long len);
    

    msg.setAddress("/sys/ready");
    msg.send(dump);
    slipOut.encode(dump.buffer, dump.length);
    serial.writeBuffer(slipOut.buffer, slipOut.length);

    msg.setAddress("/sys/renumber");
    msg.send(dump);
    slipOut.encode(dump.buffer, dump.length);
    serial.writeBuffer(slipOut.buffer, slipOut.length);


    printf("\ndone, now gonna receive\n");

    for (;;){
        // upd recive
        /*len = udpinsock.readBuffer(osc_packet_in, 256, 0);
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
        }*/

        len = serial.readBuffer(osc_packet_in, 256);
        if (len == -1) {
            //    printf("Error reading from serial port\n");
        }
        else if (len == 0) {
            //printf("No more data\n");
        }
        else
        {
            for (i = 0; i < len; i++) {
               
                uint8_t tmp8 = osc_packet_in[i];
            
                if (rstate == WAITING) {
                    if (tmp8 == eot) rstate = WAITING; // just keep waiting for something afer EOT
                    else {
                        serialOSCIn.start();
                        serialOSCIn.write(tmp8);
                        rstate = RECEIVING;
                    }
                } // waiting
                else if (rstate == RECEIVING){
                    if (tmp8 == eot) {  //TODO:  exit if message len > max
                        serialOSCIn.end();
                        printf("got a packet\n");
                        slipIn.decode(serialOSCIn.buffer, serialOSCIn.length);
                        udpoutsock.writeBuffer(slipIn.buffer, slipIn.length);
                        rstate = WAITING;
                    }
                    else {
                        serialOSCIn.write(tmp8);
                        rstate = RECEIVING;
                        /*encoded_in_index++;
                        if (encoded_in_index > (MAX_MESSAGE_SIZE - 1)){  // drop packet if it gets too large
                            rstate = WAITING;
                        }
                        else {
                            encoded_in[encoded_in_index] = tmp8;
                            rstate = RECEIVING;
                        }*/
                    }
                } //receiving
            } // gettin bytes
        } // bytes available
    } // for;;
}
