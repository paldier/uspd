#ifndef OPERATE_H
#define OPERATE_H

#include "common.h"

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

opr_ret_t create_operate_response(struct blob_buf *bb, char *cmd, struct
				  blob_attr *bv);
#endif /* OPERATE_H */
