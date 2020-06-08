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
	void *array, *table;

	pathnode *p=head;

	DEBUG("entry");

	array = blobmsg_open_array(bb, "parameters");
	while(p!=NULL) {
		struct dmctx dm_ctx = {0};
		struct dm_parameter *n;
		char tmp[NAME_MAX] = {'\0'};
		snprintf(tmp, NAME_MAX, "%s%s", p->ref_path, cmd);
		bbf_init(&dm_ctx, tmp);

		if(bv) {
			input_params = blobmsg_format_json(bv, true);
			fault = bbf_operate(&dm_ctx, tmp, input_params);
			free(input_params);
		} else {
			fault = bbf_operate(&dm_ctx, tmp, NULL);
		}

		switch(fault) {
			case CMD_NOT_FOUND:
				{
					table = blobmsg_open_table(bb, NULL);
					blobmsg_add_u32(bb, "fault", 7026); //USP_ERR_INVALID_PATH
					blobmsg_close_table(bb, table);
				}
				break;
			case UBUS_INVALID_ARGUMENTS:
				{
					table = blobmsg_open_table(bb, NULL);
					blobmsg_add_u32(bb, "fault", 7004); //USP_ERR_INVALID_ARGUMENTS
					blobmsg_close_table(bb, table);
				}
				break;
			case SUCCESS:
				{
					list_for_each_entry(n, &dm_ctx.list_parameter, list) {
						table = blobmsg_open_table(bb, NULL);
						DEBUG("insert node|%s|, value|%s| ", n->name, n->data);
						blobmsg_add_string(bb, "parameter", n->name);
						blobmsg_add_string(bb, "value", n->data);
						blobmsg_add_string(bb, "type", n->type);
						blobmsg_close_table(bb, table);
					}
				}
				break;
			case FAIL:
				{
					table = blobmsg_open_table(bb, NULL);
					blobmsg_add_u32(bb, "fault", 7022); //USP_ERR_COMMAND_FAILURE
					blobmsg_close_table(bb, table);
				}
				break;
			default:
				{
					table = blobmsg_open_table(bb, NULL);
					blobmsg_add_u32(bb, "fault", 7026); //USP_ERR_INVALID_PATH
					blobmsg_close_table(bb, table);
					ERR("Case not defined");
				}
		}
		bbf_cleanup(&dm_ctx);
		p=p->next;
	}
	blobmsg_close_array(bb, array);
	deleteList();
	return SUCCESS;
}
