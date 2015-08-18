
#ifdef __cplusplus
extern "C" {
#endif
	
#define PD_RECV_PORT 4001
#define PD_RECV_BUFSIZE 4096

int pd_receive_init(void);

int udpread(char *buf);
int pd_receive_poll(char *buf);
	
#ifdef __cplusplus
}
#endif


