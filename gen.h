#define GET_HOSTNAME 0x01
#define GET_TIME 0x02
#define GET_USER 0x03
#define GET_OS 0x04

struct cmd_msg {
	int cmd;
	char message[32];
};
#define msg_len sizeof(struct cmd_msg)
