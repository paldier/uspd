/*
 * main.c: USP deamon
 *
 * Copyright (C) 2019 iopsys Software Solutions AB. All rights reserved.
 *
 * Author: Vivek Dutta <v.dutta@gxgroup.eu>
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
 
#include <stdio.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <libubox/blobmsg.h>
#include <libubox/uloop.h>
#include <libubus.h>

#include "set.h"
#include "get.h"
#include "operate.h"
#include "common.h"


static const struct blobmsg_policy dm_get_policy[__DM_MAX] = {
	[DMPATH_NAME] = { .name = "path", .type = BLOBMSG_TYPE_STRING },
};

static int usp_get(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	DEBUG("Entry");
	struct blob_attr *tb[__DM_MAX];
	char path[PATH_MAX] = {'\0'};
	struct blob_buf bb;

	if(blobmsg_parse(dm_get_policy, __DM_MAX, tb, blob_data(msg), blob_len(msg))) {
		ERR("Failed to parse blob");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	if (!(tb[DMPATH_NAME]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	blob_buf_init(&bb, 0);
	blobmsg_buf_init(&bb);

	strcpy(path, blobmsg_data(tb[DMPATH_NAME]));
	DEBUG("Path |%s|",path);

	filter_results(path, 0, strlen(path));
	create_response(&bb);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

const struct blobmsg_policy dm_set_policy[__DMSET_MAX] = {
	[DM_SET_PATH] = { .name = "path", .type = BLOBMSG_TYPE_STRING },
	[DM_SET_VALUE] = { .name = "value", .type = BLOBMSG_TYPE_STRING },
};
int usp_set(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	DEBUG("Entry");
	struct blob_attr *tb[__DMSET_MAX];
	char path[PATH_MAX]={'\0'}, value[NAME_MAX]={'\0'};
	struct blob_buf bb;

	blobmsg_buf_init(&bb);
	blob_buf_init(&bb, 0);

	if(blobmsg_parse(dm_set_policy, __DMSET_MAX, tb, blob_data(msg), blob_len(msg))) {
		ERR("Failed to parse blob");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	if (!(tb[DM_SET_PATH]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!tb[DM_SET_VALUE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	strcpy(path, blobmsg_data(tb[DM_SET_PATH]));
	strcpy(value, blobmsg_data(tb[DM_SET_VALUE]));
	DEBUG("set path |%s| value |%s|",path, value);

	filter_results(path, 0, strlen(path));
	create_set_response(&bb, value);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

static const struct blobmsg_policy dm_operate_policy[__DM_OPERATE_MAX] = {
	[DM_OPERATE_PATH] = { .name = "path", .type = BLOBMSG_TYPE_STRING },
};

int usp_operate(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[__DM_OPERATE_MAX];
	struct blob_buf bb;
	char path[PATH_MAX]={'\0'};
	char rel_path[PATH_MAX]={'\0'};
	char cmd[NAME_MAX]={'\0'};
	char *op_path = NULL;
	DEBUG("Entry");

	if(blobmsg_parse(dm_operate_policy, __DM_OPERATE_MAX, tb, blob_data(msg), blob_len(msg))) {
		ERR("Failed to parse blob");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	if (!(tb[DM_OPERATE_PATH]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	blob_buf_init(&bb, 0);
	blobmsg_buf_init(&bb);


	strcpy(path, blobmsg_data(tb[DM_OPERATE_PATH]));

	op_path = strrchr(path, '.');
	strcpy(cmd, op_path+1);

	strncpy(rel_path, path, op_path-path+1);
	filter_results(rel_path, 0, strlen(rel_path));

	create_operate_response(&bb, cmd);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

static struct ubus_method usp_methods[] = {
	UBUS_METHOD("get", usp_get, dm_get_policy),
	UBUS_METHOD("set", usp_set, dm_set_policy),
	UBUS_METHOD("operate", usp_operate, dm_operate_policy),
};

static struct ubus_object_type usp_type = UBUS_OBJECT_TYPE("usp", usp_methods);

static struct ubus_object usp_object = {
	.name = "usp",
	.type = &usp_type,
	.methods = usp_methods,
	.n_methods = ARRAY_SIZE(usp_methods),
};

static void usp_init(struct ubus_context *ctx)
{
	int ret;

	ret = ubus_add_object(ctx, &usp_object);
	if (ret)
		ERR("Failed to add 'usp' ubus object: %s\n", ubus_strerror(ret));

	uloop_run();
}

int main()
{
	const char *ubus_socket = NULL;
	struct ubus_context *ctx = NULL;

	openlog("uspd", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	uloop_init();
	ctx = ubus_connect(ubus_socket);
	if (!ctx) {
		fprintf(stderr, "Failed to connect to ubus\n");
		return -1;
	}
	ubus_add_uloop(ctx);
	usp_init(ctx);

	uloop_run();
	ubus_free(ctx);
	uloop_done();
	closelog();

	return 0;
}
