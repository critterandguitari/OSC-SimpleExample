
#ifdef __cplusplus
extern "C" {
#endif
	
#define UDP_SEND_PORT 4000

// send the line to pd
int udp_send_buf(uint8_t * line, int len);

int udp_send_init(void);

	
#ifdef __cplusplus
}
#endif


