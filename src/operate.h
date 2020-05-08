#ifndef OPERATE_H
#define OPERATE_H

#include "common.h"

enum {
	DM_OPERATE_PATH,
	DM_OPERATE_ACTION,
	DM_OPERATE_INPUT,
	DM_OPERATE_PROTO,
	__DM_OPERATE_MAX,
};

opr_ret_t create_operate_response(struct blob_buf *bb, char *cmd, struct blob_attr *bv);
#endif /* OPERATE_H */
