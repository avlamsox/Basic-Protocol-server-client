#define GET_HOSTNAME 1
struct cmd_msg {
	int cmd;
	char message[10];
};
#define msg_len sizeof(struct cmd_msg)
