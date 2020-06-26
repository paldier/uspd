/*
 * main.c: USP deamon
 *
 * Copyright (C) 2019 iopsys Software Solutions AB. All rights reserved.
 *
 * Author: Vivek Dutta <vivek.dutta@iopsys.eu>
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

#include <stdio.h>
#include <stdlib.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <libubox/blobmsg.h>
#include <libubox/uloop.h>
#include <libubus.h>

/* Not all libc implementations have MAXNAMLEN defined
 */
#ifndef MAXNAMLEN
#define MAXNAMLEN 512
#endif

#include "set.h"
#include "get.h"
#include "operate.h"
#include "common.h"
#include "add_delete.h"
#include "strncpyt.h"

#define USP "usp"
#define USP_GRA "usp."
#define RAWUSP "usp.raw"
#define DEFAULT_LOG_LEVEL (2)

#define MIN_LEN (7)  // len of 'Device.'
#define ROOT_NODE "Device."


extern pathnode *head;

enum {
	DM_GET_PATH,
	DM_GET_PROTO,
	DM_GET_MAXDEPTH,
	DM_GET_NXT_LVL,
	__DM_GET_MAX
};

enum {
	DM_GET_SAFE_PATHS,
	DM_GET_SAFE_PROTO,
	DM_GET_SAFE_NXT_LVL,
	__DM_GET_SAFE_MAX
};

enum {
	DM_ADD_PATH,
	DM_ADD_PROTO,
	DM_ADD_PARAMETER_KEY,
	__DM_ADD_MAX
};

enum {
	DM_SET_PATH,
	DM_SET_VALUE,
	DM_SET_VALUE_TABLE,
	DM_SET_PROTO,
	DM_SET_PARAMETER_KEY,
	__DM_SET_MAX,
};

static const struct blobmsg_policy dm_get_policy[__DM_GET_MAX] = {
	[DM_GET_PATH] = { .name = "path", .type = BLOBMSG_TYPE_STRING },
	[DM_GET_PROTO] = { .name = "proto", .type = BLOBMSG_TYPE_STRING },
	[DM_GET_MAXDEPTH] = { .name = "maxdepth", .type = BLOBMSG_TYPE_INT32 },
	[DM_GET_NXT_LVL] = { .name = "next-level", .type = BLOBMSG_TYPE_INT8 },
};

static const struct blobmsg_policy dm_get_safe_policy[__DM_GET_SAFE_MAX] = {
	[DM_GET_SAFE_PATHS] = { .name = "paths", .type = BLOBMSG_TYPE_ARRAY },
	[DM_GET_SAFE_PROTO] = { .name = "proto", .type = BLOBMSG_TYPE_STRING },
	[DM_GET_SAFE_NXT_LVL] = { .name = "next-level", .type = BLOBMSG_TYPE_INT8 },
};

static const struct blobmsg_policy dm_add_policy[__DM_ADD_MAX] = {
	[DM_ADD_PATH] = { .name = "path", .type = BLOBMSG_TYPE_STRING },
	[DM_ADD_PROTO] = { .name = "proto", .type = BLOBMSG_TYPE_STRING },
	[DM_ADD_PARAMETER_KEY] = { .name = "key", .type = BLOBMSG_TYPE_STRING }
};

static const struct blobmsg_policy dm_set_policy[__DM_SET_MAX] = {
	[DM_SET_PATH] = { .name = "path", .type = BLOBMSG_TYPE_STRING },
	[DM_SET_VALUE] = { .name = "value", .type = BLOBMSG_TYPE_STRING },
	[DM_SET_VALUE_TABLE] = { .name = "values", .type = BLOBMSG_TYPE_TABLE },
	[DM_SET_PROTO] = { .name = "proto", .type = BLOBMSG_TYPE_STRING },
	[DM_SET_PARAMETER_KEY] = { .name = "key", .type = BLOBMSG_TYPE_STRING }
};

static bool is_sanitized(char *param)
{
	size_t len;

	if (param==NULL)
		return false;

	len = strlen(param);
	if (param[0] == '\0' || param[0] == ' ')
		return false;

	// Check the minimum len in path
	if (len < MIN_LEN)
		return false;

	// Fail if path does not start from Device.
	if (strncmp(param, ROOT_NODE, MIN_LEN))
		return false;

	if (param[len-1]==' ')
		return false;

	return true;
}

static void set_bbf_data_type(struct blob_attr *proto)
{
	int type;

	if (proto) {
		const char *val = blobmsg_get_string(proto);

		if (is_str_eq("cwmp", val))
			type = BBFDM_CWMP;
		else if (is_str_eq("usp", val))
			type = BBFDM_USP;
		else
			type = BBFDM_BOTH;
	} else {
		type = BBFDM_BOTH;
	}

	set_bbfdatamodel_type(type);
}

static int get_safe(struct ubus_context *ctx,
			struct ubus_object *obj,
			struct ubus_request_data *req,
			struct blob_attr *msg,
			int bbf_cmd)
{
	struct blob_buf bb = {};
	struct blob_attr *tb[__DM_GET_SAFE_MAX];
	struct blob_attr *paths;
	struct blob_attr *path;
	void *a;
	char *nxt_lvl;
	size_t rem;
	const int raw = is_str_eq(obj->name, RAWUSP);

	blobmsg_parse(dm_get_safe_policy, __DM_GET_SAFE_MAX,
			tb, blob_data(msg), blob_len(msg));

	paths = tb[DM_GET_SAFE_PATHS];
	if (paths == NULL)
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (tb[DM_GET_SAFE_NXT_LVL]) {
		if (blobmsg_get_u8(tb[DM_GET_SAFE_NXT_LVL]))
			nxt_lvl = "1";
		else
			nxt_lvl = "0";
	} else {
		nxt_lvl = NULL;
	}

	set_bbf_data_type(tb[DM_GET_SAFE_PROTO]);

	memset(&bb, 0, sizeof(struct blob_buf));
	blob_buf_init(&bb, 0);

	if (raw)
		a = blobmsg_open_array(&bb, "parameters");

	blobmsg_for_each_attr(path, paths, rem) {
		char *path_str = blobmsg_get_string(path);

		if (raw)
			bbf_get_raw(bbf_cmd, path_str, &bb, nxt_lvl);
		else
			bbf_get_blob(bbf_cmd, path_str, &bb, nxt_lvl);
	}

	if (raw)
		blobmsg_close_array(&bb, a);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);
	return 0;
}

static int usp_get_attributes(struct ubus_context *ctx,
			struct ubus_object *obj,
			struct ubus_request_data *req,
			__unused const char *method,
			struct blob_attr *msg)
{
	return get_safe(ctx, obj, req, msg, CMD_GET_NOTIFICATION);
}

static int usp_get_safe_values(struct ubus_context *ctx,
			struct ubus_object *obj,
			struct ubus_request_data *req,
			__unused const char *method,
			struct blob_attr *msg)
{
	return get_safe(ctx, obj, req, msg, CMD_GET_VALUE);
}

static int usp_get_safe_names(struct ubus_context *ctx,
			struct ubus_object *obj,
			struct ubus_request_data *req,
			__unused const char *method,
			struct blob_attr *msg)
{
	return get_safe(ctx, obj, req, msg, CMD_GET_NAME);
}


int usp_add_del_handler(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[__DM_ADD_MAX];
	char path[PATH_MAX];
	const char *pkey = NULL;
	struct blob_buf bb = {};
	size_t path_len;
	char *blob_msg = NULL;


	if(blobmsg_parse(dm_add_policy, __DM_ADD_MAX, tb, blob_data(msg), blob_len(msg))) {
		ERR("Failed to parse blob");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	if (!(tb[DM_ADD_PATH]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	set_bbf_data_type(tb[DM_ADD_PROTO]);

	// In case of granular objects, Concatenate relative path to ubus object
	if ((0 != strcmp(obj->name, USP)) && (0 != strcmp(obj->name, RAWUSP))) {
		snprintf(path, NAME_MAX, "%s.%s", obj->name+strlen(USP_GRA), \
			(char *)blobmsg_data(tb[DM_ADD_PATH]));
	} else {
		blob_msg = blobmsg_data(tb[DM_ADD_PATH]);
		strncpyt(path, blob_msg, sizeof(path));
	}

	path_len = strlen(path);
	// Path is invalid if the path have leave elements
	// Need to add . as obuspa trims last . from path
	if(path[path_len-1] != DELIM) {
		strcat(path, ".");
	}

	if (!is_sanitized(path)) {
		ERR("Invalid path |%s|", path);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	if (tb[DM_ADD_PARAMETER_KEY])
		pkey = blobmsg_get_string(tb[DM_ADD_PARAMETER_KEY]);

	memset(&bb, 0, sizeof(struct blob_buf));
	blob_buf_init(&bb, 0);

	INFO("ubus method|%s| name|%s|, path(%s)", method, obj->name, path);

	if (is_str_eq(method, "add_object")) {
		add_object(&bb, path, pkey);
	} else if (is_str_eq(method, "del_object")) {
		create_del_response(&bb, path, pkey);
	} else {
		ERR("method(%s) not defined", method);
	}

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

int usp_get_handler(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[__DM_GET_MAX];
	char path[PATH_MAX];
	struct blob_buf bb = {};
	char *blob_msg = NULL;
	uint8_t maxdepth = 0;


	if(blobmsg_parse(dm_get_policy, __DM_GET_MAX, tb, blob_data(msg), blob_len(msg))) {
		ERR("Failed to parse blob");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	if (!(tb[DM_GET_PATH]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	set_bbf_data_type(tb[DM_GET_PROTO]);

	// In case of granular objects, Concatenate relative path to ubus object
	if ((0 != strcmp(obj->name, USP)) && (0 != strcmp(obj->name, RAWUSP))) {
		snprintf(path, NAME_MAX, "%s.%s", obj->name+strlen(USP_GRA), \
			(char *)blobmsg_data(tb[DM_GET_PATH]));
	} else {
		blob_msg = blobmsg_data(tb[DM_GET_PATH]);
		strncpyt(path, blob_msg, sizeof(path));
	}

	if (!is_sanitized(path)) {
		ERR("Invalid path |%s|", path);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	INFO("ubus method|%s|, name|%s|, path(%s)", method, obj->name, path);

	filter_results(path, 0, strlen(path));
	update_valid_paths();

	if (tb[DM_GET_MAXDEPTH])
		maxdepth = blobmsg_get_u32(tb[DM_GET_MAXDEPTH]);

	memset(&bb, 0, sizeof(struct blob_buf));
	blob_buf_init(&bb, 0);

	if (is_str_eq(method, "get")) {
		if(is_str_eq(obj->name, RAWUSP)) {
			create_raw_response(&bb);
		} else {
			create_response(&bb, path, maxdepth);
		}
	} else if (is_str_eq(method, "object_names")) {
		bool nxt_lvl;

		if (tb[DM_GET_NXT_LVL])
			nxt_lvl = blobmsg_get_u8(tb[DM_GET_NXT_LVL]);
		else
			nxt_lvl = false;

		create_name_response(&bb, nxt_lvl);
	} else if (is_str_eq(method, "instances")) {
		create_inst_name_response(&bb);
	} else if (is_str_eq(method, "resolve")) {
		get_resolved_path(&bb);
	} else {
		ERR("method(%s) not defined", method);
	}

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

int usp_set(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_buf bb = {};
	struct blob_attr *tb[__DM_SET_MAX] = {NULL};
	char path[PATH_MAX]={'\0'}, value[NAME_MAX]={'\0'};
	char *blob_msg = NULL;
	void *array = NULL;
	char *key = "";

	if(blobmsg_parse(dm_set_policy, __DM_SET_MAX, tb, blob_data(msg), blob_len(msg))) {
		ERR("Failed to parse blob");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	if (!tb[DM_SET_PATH])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!tb[DM_SET_VALUE] && !tb[DM_SET_VALUE_TABLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (tb[DM_SET_PARAMETER_KEY])
		key = blobmsg_get_string(tb[DM_SET_PARAMETER_KEY]);

	set_bbf_data_type(tb[DM_SET_PROTO]);

	memset(&bb, 0, sizeof(struct blob_buf));
	blob_buf_init(&bb, 0);

	// In case of granular objects, Concatenate relative path to ubus object
	if ((0 != strcmp(obj->name, USP)) && (0 != strcmp(obj->name, RAWUSP))) {
		snprintf(path, NAME_MAX, "%s.%s", obj->name+strlen(USP_GRA), \
			 (char *)blobmsg_data(tb[DM_SET_PATH]));
	} else {
		blob_msg = blobmsg_data(tb[DM_SET_PATH]);
		strncpyt(path, blob_msg, sizeof(path));
	}

	if (!is_sanitized(path)) {
		ERR("Invalid path |%s|", path);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	INFO("ubus method|%s|, name|%s|, path(%s)", method, obj->name, path);
	filter_results(path, 0, strlen(path));
	update_valid_paths();

	array = blobmsg_open_array(&bb, "parameters");

	if (tb[DM_SET_VALUE]) {
		blob_msg = blobmsg_data(tb[DM_SET_VALUE]);
		strncpyt(value, blob_msg, sizeof(value));
		create_set_response(&bb, value, key);
	}
	if(tb[DM_SET_VALUE_TABLE]) {
		set_multiple_values(&bb, tb[DM_SET_VALUE_TABLE], key);
	}

	blobmsg_close_array(&bb, array);
	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	// Restart all the affected services
	dm_entry_restart_services();
	return 0;
}

static const struct blobmsg_policy dm_operate_policy[__DM_OPERATE_MAX] = {
	[DM_OPERATE_PATH] = { .name = "path", .type = BLOBMSG_TYPE_STRING },
	[DM_OPERATE_ACTION] = { .name = "action", .type = BLOBMSG_TYPE_STRING },
	[DM_OPERATE_INPUT] = { .name = "input", .type = BLOBMSG_TYPE_TABLE },
	[DM_OPERATE_PROTO] = { .name = "proto", .type = BLOBMSG_TYPE_STRING }
};

int usp_operate(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[__DM_OPERATE_MAX] = {NULL};
	struct blob_buf bb;
	char path[PATH_MAX]={'\0'};
	char cmd[NAME_MAX]={'\0'};
	char *blob_msg = NULL;

	if(blobmsg_parse(dm_operate_policy, __DM_OPERATE_MAX, tb, blob_data(msg), blob_len(msg))) {
		ERR("Failed to parse blob");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	if (!(tb[DM_OPERATE_PATH]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!(tb[DM_OPERATE_ACTION]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	set_bbf_data_type(tb[DM_OPERATE_PROTO]);

	memset(&bb, 0, sizeof(struct blob_buf));
	blob_buf_init(&bb, 0);

	// In case of granular objects, Concatenate relative path to ubus object
	if ((0 != strcmp(obj->name, USP)) && (0 != strcmp(obj->name, RAWUSP))) {
		snprintf(path, NAME_MAX, "%s.%s", obj->name+strlen(USP_GRA), \
			 (char *)blobmsg_data(tb[DM_OPERATE_PATH]));
	} else {
		blob_msg = blobmsg_data(tb[DM_OPERATE_PATH]);
		strncpyt(path, blob_msg, sizeof(path));
	}
	blob_msg = blobmsg_data(tb[DM_OPERATE_ACTION]);
	strncpyt(cmd, blob_msg, sizeof(cmd));
	INFO("ubus method|%s|, name|%s|, path(%s)", method, obj->name, path);

	filter_results(path, 0, strlen(path));
	update_valid_paths();

	if(UBUS_INVALID_ARGUMENTS == create_operate_response(&bb, cmd, tb[DM_OPERATE_INPUT])) {
		blob_buf_free(&bb);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

static struct ubus_method usp_methods[] = {
	UBUS_METHOD("get", usp_get_handler, dm_get_policy),
	UBUS_METHOD("get_safe_attributes", usp_get_attributes, dm_get_safe_policy),
	UBUS_METHOD("get_safe_values", usp_get_safe_values, dm_get_safe_policy),
	UBUS_METHOD("get_safe_names", usp_get_safe_names, dm_get_safe_policy),
	UBUS_METHOD("set", usp_set, dm_set_policy),
	UBUS_METHOD("operate", usp_operate, dm_operate_policy),
	UBUS_METHOD("add_object", usp_add_del_handler, dm_add_policy),
	UBUS_METHOD("del_object", usp_add_del_handler, dm_add_policy),
	UBUS_METHOD("object_names", usp_get_handler, dm_get_policy),
	UBUS_METHOD("instances", usp_get_handler, dm_get_policy),
	UBUS_METHOD("resolve", usp_get_handler, dm_get_policy),
};

static struct ubus_object_type usp_type = UBUS_OBJECT_TYPE("usp", usp_methods);

static struct ubus_object usp_object = {
	.name = USP,
	.type = &usp_type,
	.methods = usp_methods,
	.n_methods = ARRAY_SIZE(usp_methods),
};

static struct ubus_object usp_raw_object = {
	.name = RAWUSP,
	.type = &usp_type,
	.methods = usp_methods,
	.n_methods = ARRAY_SIZE(usp_methods),
};

static void ubus_add_granular_objects(struct ubus_context *ctx, char *obj)
{
	int retval;
	char *obj_name = strdup(obj);
	struct ubus_object *usp_granular_object = (struct ubus_object*)
			malloc(sizeof(struct ubus_object));

	struct ubus_object granular_object = {
		.name = obj_name,
		.type = &usp_type,
		.methods = usp_methods,
		.n_methods = ARRAY_SIZE(usp_methods)};

	*usp_granular_object = granular_object;

	retval = ubus_add_object(ctx, usp_granular_object);
	if (retval)
		ERR("Failed to add 'usp' ubus object: %s\n", ubus_strerror(retval));

}

static void add_granular_objects(struct ubus_context *ctx, int gn_level)
{
	pathnode *p=head;

	if (gn_level == 0) {
		INFO("Granularity level is set to zero. No more 'usp' ubus objects to add");
		return;
	}

	set_bbfdatamodel_type(BBFDM_BOTH);

	insert(strdup("Device."), true);
	for (int i = 2; i <= gn_level; i++) {
		p = head;
		while(p!=NULL) {
			get_granular_obj_list(p->ref_path);
			p=p->next;
		}
	}
	p = head;
	while(p!=NULL) {
		char obj_path[MAXNAMLEN]=USP_GRA;
		strncat(obj_path,p->ref_path,sizeof(obj_path) - 1);

		size_t len = strlen(obj_path);
		if (obj_path[len-1] == DELIM)
			obj_path[len-1]='\0';

		DEBUG("ubus objects to register |%s|", obj_path);
		ubus_add_granular_objects(ctx, obj_path);
		p=p->next;
	}
	deleteList();
}

static void usp_init(struct ubus_context *ctx)
{
	int ret;

	char *log_level = NULL;
	if (!(get_uci_option_string("uspd", "usp", "loglevel", &log_level))) {
		INFO("loglevel get failed, defaults to|0x%x|", DEFAULT_LOG_LEVEL);
		set_debug_level(DEFAULT_LOG_LEVEL);
	} else {
		set_debug_level(atoi(log_level));
		free(log_level);
	}

	ret = ubus_add_object(ctx, &usp_object);
	if (ret) {
		ERR("Failed to add 'usp' ubus object: %s\n", ubus_strerror(ret));
		exit(ret);
	}

	ret = ubus_add_object(ctx, &usp_raw_object);
	if (ret) {
		ERR("Failed to add 'usp' ubus object: %s\n", ubus_strerror(ret));
		exit(ret);
	}

	// Get Granularity level of 'uspd' ubus objects
	char *gran_level = NULL;

	if (!(get_uci_option_string("uspd", "usp", "granularitylevel",
				   &gran_level))) {
		INFO("Failed to fetch uci option 'granularitylevel'");
	} else {

		int gn_level = atoi(gran_level);
		DEBUG("Granularity level is %d", gn_level);
		if (gn_level < 0)
			WARNING("Granularity Level should either be zero or a positive number");
		else
			add_granular_objects(ctx, gn_level);

		free(gran_level);
	}
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
