#ifndef OPERATE_H
#define OPERATE_H

#define SYSTEM_UBUS_PATH "system"
#define NETWORK_INTERFACE_UBUS_PATH "network.interface"
#define NETWORK_WIRELESS_UBUS_PATH "network.wireless"

enum {
	DM_OPERATE_PATH,
	DM_OPERATE_ACTION,
	__DM_OPERATE_MAX,
};

typedef void (*operation) (char *p);

struct op_cmd {
	char *name;
	operation opt;
};

void create_operate_response(struct blob_buf *bb, char *cmd);
#endif /* OPERATE_H */
