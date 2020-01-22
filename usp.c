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
#include "add_delete.h"

#define USP "usp"
#define USP_GRA "usp."
#define RAWUSP "usp.raw"
extern pathnode *head;

static bool is_sanitized(char *param)
{
	if (param==NULL)
		return false;

	size_t len = strlen(param);
	if (param[0]=='\0' || param[0]==' ' || len<1)
		return false;

	if (param[len-1]==' ')
		return false;

	return true;
}

static const struct blobmsg_policy dm_get_policy[__DM_MAX] = {
	[DMPATH_NAME] = { .name = "path", .type = BLOBMSG_TYPE_STRING }
};

static int usp_get(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	DEBUG("Entry method|%s| ubus name|%s|", method, obj->name);
	struct blob_attr *tb[__DM_MAX] = {NULL};
	char path[PATH_MAX] = {'\0'};

	if(blobmsg_parse(dm_get_policy, __DM_MAX, tb, blob_data(msg), blob_len(msg))) {
		ERR("Failed to parse blob");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	if (!(tb[DMPATH_NAME]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!is_sanitized(blobmsg_data(tb[DMPATH_NAME]))) {
		ERR("Invalid option |%s|", (char *)blobmsg_data(tb[DMPATH_NAME]));
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	struct blob_buf bb;
	memset(&bb,0,sizeof(struct blob_buf));
	blob_buf_init(&bb, 0);

	// In case of granular objects, Concatenate relative path to ubus object
	if ((0 != strcmp(obj->name, USP)) && (0 != strcmp(obj->name, RAWUSP))) {
		snprintf(path, NAME_MAX, "%s.%s", obj->name+strlen(USP_GRA), \
			(char *)blobmsg_data(tb[DMPATH_NAME]));
	} else {
		strcpy(path, blobmsg_data(tb[DMPATH_NAME]));
	}

	filter_results(path, 0, strlen(path));
	if(is_str_eq(obj->name, RAWUSP)) {
		create_raw_response(&bb);
	} else {
		create_response(&bb, path);
	}

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

static int usp_add(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	DEBUG("Entry method|%s| ubus name|%s|", method, obj->name);
	struct blob_attr *tb[__DM_MAX] = {NULL};
	char path[PATH_MAX] = {'\0'};

	if(blobmsg_parse(dm_get_policy, __DM_MAX, tb, blob_data(msg), blob_len(msg))) {
		ERR("Failed to parse blob");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	if (!(tb[DMPATH_NAME]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!is_sanitized(blobmsg_data(tb[DMPATH_NAME]))) {
		ERR("Invalid option |%s|", (char *)blobmsg_data(tb[DMPATH_NAME]));
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	struct blob_buf bb;
	memset(&bb,0,sizeof(struct blob_buf));
	blob_buf_init(&bb, 0);

	// In case of granular objects, Concatenate relative path to ubus object
	if ((0 != strcmp(obj->name, USP)) && (0 != strcmp(obj->name, RAWUSP))) {
		snprintf(path, NAME_MAX, "%s.%s", obj->name+strlen(USP_GRA), \
			(char *)blobmsg_data(tb[DMPATH_NAME]));
	} else {
		strcpy(path, blobmsg_data(tb[DMPATH_NAME]));
	}
	size_t path_len = strlen(path);
	// Path is invalid if the path have leave elements
	// Need to add . as obuspa trims last . from path
	if(path[path_len-1] != DELIM) {
		strcat(path, ".");
	}

	add_object(&bb, path);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

static int usp_object_name(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	DEBUG("Entry method|%s| ubus name|%s|", method, obj->name);
	struct blob_attr *tb[__DM_MAX] = {NULL};
	char path[PATH_MAX] = {'\0'};

	if(blobmsg_parse(dm_get_policy, __DM_MAX, tb, blob_data(msg), blob_len(msg))) {
		ERR("Failed to parse blob");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	if (!(tb[DMPATH_NAME]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!is_sanitized(blobmsg_data(tb[DMPATH_NAME]))) {
		ERR("Invalid option |%s|", (char *)blobmsg_data(tb[DMPATH_NAME]));
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	struct blob_buf bb;
	memset(&bb,0,sizeof(struct blob_buf));
	blob_buf_init(&bb, 0);

	// In case of granular objects, Concatenate relative path to ubus object
	if ((0 != strcmp(obj->name, USP)) && (0 != strcmp(obj->name, RAWUSP))) {
		snprintf(path, NAME_MAX, "%s.%s", obj->name+strlen(USP_GRA), \
			(char *)blobmsg_data(tb[DMPATH_NAME]));
	} else {
		strcpy(path, blobmsg_data(tb[DMPATH_NAME]));
	}

	filter_results(path, 0, strlen(path));
	create_name_response(&bb);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

static int usp_del(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	DEBUG("Entry method|%s| ubus name|%s|", method, obj->name);
	struct blob_attr *tb[__DM_MAX] = {NULL};
	char path[PATH_MAX] = {'\0'};

	if(blobmsg_parse(dm_get_policy, __DM_MAX, tb, blob_data(msg), blob_len(msg))) {
		ERR("Failed to parse blob");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	if (!(tb[DMPATH_NAME]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!is_sanitized(blobmsg_data(tb[DMPATH_NAME]))) {
		ERR("Invalid option |%s|", (char *)blobmsg_data(tb[DMPATH_NAME]));
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	struct blob_buf bb;
	memset(&bb,0,sizeof(struct blob_buf));
	blob_buf_init(&bb, 0);

	// In case of granular objects, Concatenate relative path to ubus object
	if ((0 != strcmp(obj->name, USP)) && (0 != strcmp(obj->name, RAWUSP))) {
		snprintf(path, NAME_MAX, "%s.%s", obj->name+strlen(USP_GRA), \
			(char *)blobmsg_data(tb[DMPATH_NAME]));
	} else {
		strcpy(path, blobmsg_data(tb[DMPATH_NAME]));
	}

	size_t path_len = strlen(path);
	// Path is invalid if the path have leave elements
	// Need to add . as obuspa trims last . from path
	if(path[path_len-1] != DELIM) {
		strcat(path, ".");
	}

	filter_results(path, 0, strlen(path));
	pathnode *p=head;
	while(p) {
		del_object(&bb, p->ref_path);
		p = p->next;
	}

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

const struct blobmsg_policy dm_set_policy[__DMSET_MAX] = {
	[DM_SET_PATH] = { .name = "path", .type = BLOBMSG_TYPE_STRING },
	[DM_SET_VALUE] = { .name = "value", .type = BLOBMSG_TYPE_STRING },
	[DM_SET_VALUE_TABLE] = { .name = "values", .type = BLOBMSG_TYPE_TABLE }
};
int usp_set(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	DEBUG("Entry method|%s| ubus name|%s|", method, obj->name);
	struct blob_attr *tb[__DMSET_MAX] = {NULL};
	char path[PATH_MAX]={'\0'}, value[NAME_MAX]={'\0'};

	if(blobmsg_parse(dm_set_policy, __DMSET_MAX, tb, blob_data(msg), blob_len(msg))) {
		ERR("Failed to parse blob");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	if (!tb[DM_SET_PATH])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!tb[DM_SET_VALUE] && !tb[DM_SET_VALUE_TABLE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!is_sanitized(blobmsg_data(tb[DM_SET_PATH]))) {
		ERR("Invalid option |%s|", (char *)blobmsg_data(tb[DM_SET_PATH]));
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	struct blob_buf bb;
	memset(&bb,0,sizeof(struct blob_buf));

	blob_buf_init(&bb, 0);

	// In case of granular objects, Concatenate relative path to ubus object
	if ((0 != strcmp(obj->name, USP)) && (0 != strcmp(obj->name, RAWUSP))) {
		snprintf(path, NAME_MAX, "%s.%s", obj->name+strlen(USP_GRA), \
			 (char *)blobmsg_data(tb[DM_SET_PATH]));
	} else {
		strcpy(path, blobmsg_data(tb[DM_SET_PATH]));
	}

	filter_results(path, 0, strlen(path));

	void *array = NULL;
	if(is_str_eq(obj->name, RAWUSP)) {
		array = blobmsg_open_array(&bb, "parameters");
	}

	if (tb[DM_SET_VALUE]) {
		strcpy(value, blobmsg_data(tb[DM_SET_VALUE]));
		create_set_response(&bb, value);
	}
	if(tb[DM_SET_VALUE_TABLE]) {
		set_multiple_values(&bb, tb[DM_SET_VALUE_TABLE]);
	}

	if(is_str_eq(obj->name, RAWUSP)) {
		blobmsg_close_array(&bb, array);
	}
	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	// Restart all the affected services
	dm_entry_restart_services();
	return 0;
}

static const struct blobmsg_policy dm_operate_policy[__DM_OPERATE_MAX] = {
	[DM_OPERATE_PATH] = { .name = "path", .type = BLOBMSG_TYPE_STRING },
	[DM_OPERATE_ACTION] = { .name = "action", .type = BLOBMSG_TYPE_STRING },
	[DM_OPERATE_INPUT] = { .name = "input", .type = BLOBMSG_TYPE_TABLE }
};

int usp_operate(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[__DM_OPERATE_MAX] = {NULL};
	struct blob_buf bb;
	char path[PATH_MAX]={'\0'};
	char cmd[NAME_MAX]={'\0'};
	DEBUG("Entry method|%s| ubus name|%s|", method, obj->name);

	if(blobmsg_parse(dm_operate_policy, __DM_OPERATE_MAX, tb, blob_data(msg), blob_len(msg))) {
		ERR("Failed to parse blob");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	if (!(tb[DM_OPERATE_PATH]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!(tb[DM_OPERATE_ACTION]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!is_sanitized(blobmsg_data(tb[DM_OPERATE_PATH]))) {
		ERR("Invalid option |%s|", (char *)blobmsg_data(tb[DM_OPERATE_PATH]));
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	memset(&bb, 0, sizeof(struct blob_buf));
	blob_buf_init(&bb, 0);

	// In case of granular objects, Concatenate relative path to ubus object
	if ((0 != strcmp(obj->name, USP)) && (0 != strcmp(obj->name, RAWUSP))) {
		snprintf(path, NAME_MAX, "%s.%s", obj->name+strlen(USP_GRA), \
			 (char *)blobmsg_data(tb[DM_OPERATE_PATH]));
	} else {
		strcpy(path, blobmsg_data(tb[DM_OPERATE_PATH]));
	}

	strcpy(cmd, blobmsg_data(tb[DM_OPERATE_ACTION]));

	filter_results(path, 0, strlen(path));

	if(UBUS_INVALID_ARGUMENTS == create_operate_response(&bb, cmd, tb[DM_OPERATE_INPUT])) {
		blob_buf_free(&bb);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

static struct ubus_method usp_methods[] = {
	UBUS_METHOD("get", usp_get, dm_get_policy),
	UBUS_METHOD("set", usp_set, dm_set_policy),
	UBUS_METHOD("operate", usp_operate, dm_operate_policy),
	UBUS_METHOD("add_object", usp_add, dm_get_policy),
	UBUS_METHOD("del_object", usp_del, dm_get_policy),
	UBUS_METHOD("object_names", usp_object_name, dm_get_policy),
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
		DEBUG("Granularity level is set to zero. No more 'usp' ubus objects to add");
		return;
	}

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
		strcat(obj_path,p->ref_path);

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


	ret = ubus_add_object(ctx, &usp_object);
	if (ret)
		ERR("Failed to add 'usp' ubus object: %s\n", ubus_strerror(ret));

	ret = ubus_add_object(ctx, &usp_raw_object);
	if (ret)
		ERR("Failed to add 'usp' ubus object: %s\n", ubus_strerror(ret));

	// Get Granularity level of 'uspd' ubus objects
	char *gran_level = NULL;

	if (!(get_uci_option_string("uspd", "usp", "granularitylevel",
				   &gran_level))) {
		ERR("Failed to fetch uci option 'granularitylevel'");
	} else {

		int gn_level = atoi(gran_level);
		DEBUG("Granularity level is %d", gn_level);
		if (gn_level < 0)
			ERR("Granularity Level should either be zero or a positive number");
		else
			add_granular_objects(ctx, gn_level);
	}
	uloop_run();
}

int main()
{
	const char *ubus_socket = NULL;
	struct ubus_context *ctx = NULL;

	openlog("uspd", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	set_bbfdatamodel_type(BBFDM_USP); // To show only USP parameters

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
