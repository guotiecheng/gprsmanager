#ifndef  _COMPORT_H
#define   _COMPORT_H

#define  BUF_64  64


typedef	struct __COM_PORT	
{
	unsigned char		databit, parity, stopbit, flowctrl, is_connted;
	long			baudrate;
	char			dev_name[BUF_64];
	int			fd_com;
	int			frag_size;
}COM_PORT;



void init_comport(COM_PORT *comport );
unsigned char comport_open (COM_PORT *comport);
void comport_term (COM_PORT *comport);
void set_setting(char *set, COM_PORT *comport);
void disp_setting(COM_PORT *comport);
void nonblock ();
int kbhit ();
unsigned char comport_recv ( COM_PORT *comport, unsigned char *buf, int buf_size,
		                             int *recv_bytes, unsigned long timeout);
unsigned char comport_send (COM_PORT *comport, unsigned char *buf, int send_bytes);
void comport_close(COM_PORT *comport);
#endif

