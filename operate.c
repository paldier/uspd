/*
 * operate.c: Operate handler for uspd
 *
 * Copyright (C) 2019 iopsys Software Solutions AB. All rights reserved.
 *
 * Author: Yashvardhan <y.yashvardhan@iopsys.eu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "common.h"
#include "operate.h"

extern pathnode *head;

opr_ret_t create_operate_response(struct blob_buf *bb, char *cmd, struct blob_attr *bv)
{
	char *input_params;
	int fault = 0;
	bool display_op = false;

	pathnode *p=head;

	DEBUG("entry");

	while(p!=NULL) {
		struct dmctx dm_ctx = {0};
		struct dm_parameter *n;
		char tmp[NAME_MAX] = {'\0'};
		sprintf(tmp, "%s%s", p->ref_path, cmd);
		cwmp_init(&dm_ctx, tmp);

		if(bv) {
			input_params = blobmsg_format_json(bv, true);
			fault = cwmp_operate(&dm_ctx, tmp, input_params);
			free(input_params);
		} else {
			fault = cwmp_operate(&dm_ctx, tmp, NULL);
		}

		switch(fault) {
			case CMD_NOT_FOUND:
				blobmsg_add_u8(bb, "status", 0);
				blobmsg_add_string(bb, "error", "method not supported");
				break;
			case UBUS_INVALID_ARGUMENTS:
				deleteList();
				cwmp_cleanup(&dm_ctx);
				return(UBUS_INVALID_ARGUMENTS);
			case SUCCESS:
				list_for_each_entry(n, &dm_ctx.list_parameter, list) {
					DEBUG("insert node|%s|, value|%s| ", n->name, n->data);
					insert_result(n->name, n->data, n->type);
					display_op = true;
				}
				if (!display_op)
					blobmsg_add_u8(bb, "status", 1);
				break;
			case FAIL:
				blobmsg_add_u8(bb, "status", 0);
				break;
			default:
				ERR("Case not defined");
		}
		cwmp_cleanup(&dm_ctx);
		p=p->next;
	}
	deleteList();
	prepare_result(bb);
	return SUCCESS;
}
