/*
 * add_delete.c: Add/Delete handler for uspd
 *
 * Copyright (C) 2019 iopsys Software Solutions AB. All rights reserved.
 *
 * Author: Vivek Dutta <vivek.dutta@iopsys.eu>
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

void add_object(struct blob_buf *bb, char *path, const char *pkey)
{
	struct dmctx dm_ctx = {0};
	struct dmctx *ctx = &dm_ctx;
	uint32_t fault = 0;

	INFO("Req to add object |%s|", path);
	bbf_init(&dm_ctx, path);

	if (pkey == NULL || pkey[0] == 0)
		pkey = "true";

	fault = (uint32_t)dm_entry_param_method(&dm_ctx, CMD_ADD_OBJECT, path, (char *)pkey, NULL);

	if (fault) {
		blobmsg_add_u32(bb, "fault", fault);
	} else {

		if (ctx->addobj_instance) {
			blobmsg_add_u8(bb, "status", 1);
			blobmsg_add_string(bb, "instance", ctx->addobj_instance);
		} else {
			blobmsg_add_u8(bb, "status", 0);
			blobmsg_add_u32(bb, "fault", FAULT_9002);
		}

		bbf_apply_end_session();
		dm_entry_restart_services();
	}
	bbf_cleanup(&dm_ctx);
}

void del_object(struct blob_buf *bb, char *path, const char *pkey)
{
	struct dmctx dm_ctx = {0};
	uint32_t fault = 0;

	bbf_init(&dm_ctx, path);

	if (pkey == NULL || pkey[0] == 0)
		pkey = "true";

	fault = (uint32_t)dm_entry_param_method(&dm_ctx, CMD_DEL_OBJECT, path, (char *)pkey, NULL);
	if (fault) {
		blobmsg_add_u8(bb, "status", 0);
		blobmsg_add_u32(bb, "fault", fault);
	} else {
		blobmsg_add_u8(bb, "status", 1);
		bbf_apply_end_session();
		dm_entry_restart_services();
	}
	bbf_cleanup(&dm_ctx);
}
