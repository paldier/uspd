#ifndef OPERATE_H
#define OPERATE_H

#include "common.h"

#define SYSTEM_UBUS_PATH "system"
#define NETWORK_INTERFACE_UBUS_PATH "network.interface"
#define NETWORK_WIRELESS_UBUS_PATH "network.wireless"
#define ROUTER_WIRELESS_UBUS_PATH "router.wireless"

enum {
	DM_OPERATE_PATH,
	DM_OPERATE_ACTION,
	DM_OPERATE_INPUT,
	__DM_OPERATE_MAX,
};

enum operate_ret_status{
	UBUS_INVALID_ARGUMENTS,
	SUCCESS,
	FAIL,
	CMD_NOT_FOUND,
	__STATUS_MAX,
};

typedef enum operate_ret_status opr_ret_t;

typedef opr_ret_t (*operation) (struct blob_buf *bb, char *p, struct blob_attr *bv);

struct wifi_security_params {
	char node[MAXNAMLEN];
	char *param;
	char value[MAXNAMLEN];
};


struct op_cmd {
	char *name;
	operation opt;
};

opr_ret_t create_operate_response(struct blob_buf *bb, char *cmd, struct blob_attr *bv);
#endif /* OPERATE_H */
