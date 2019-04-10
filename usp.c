/*
 * usp.c: Get/Set/Operate handler for uspd
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
#include <stdlib.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubox/utils.h>
#include <libubus.h>
#include <regex.h>
#include <sys/param.h>

#include <libdatamodel/dmubus.h>
#include <libdatamodel/dmcommon.h>

#include "usp.h"

#define GLOB_CHAR "[[+*#]+"
#define MULTI_COND "&&"
#define REF_PATH "\\+"
#define DM_USE_LIBUBUS

// Global variables
struct blob_buf bb;
pathnode *head = NULL;
pathnode *temphead = NULL;

typedef void (*operation) (char *p);

struct op_cmd {
	char *name;
	operation opt;
};

void opr_reboot_device(char *p);
void opr_factory_reset(char *p);
void opr_in_progress(char *p);
void opr_network_reset(char *p);

static struct op_cmd operate_helper[] = {
	[0] = {"Device.Reboot()", opr_reboot_device},
	[1] = {"Device.FactoryReset()", opr_factory_reset},
	[2] = {"Device.IP.Interface.*.Reset()", opr_network_reset}
};

// Dummy function definations for libdatamodel binding
void puts_log(int severity, const char *fmt, ...) {
	DEBUG("Dummy funtion");
	if(severity && fmt){}
}
void add_list_value_change(char *param_name, char *param_data, char *param_type) {
	DEBUG("Dummy funtion");
	if(param_name && param_data && param_type){}
}

void send_active_value_change(void){
	DEBUG("Dummy funtion");
}

int copy_temporary_file_to_original_file(char *f1, char *f2) {
	FILE *fp, *ftmp;
	char ch;

	ftmp = fopen(f2, "r");
	if (ftmp == NULL)
		return 0;

	fp = fopen(f1, "w");
	if (fp == NULL) {
		fclose(ftmp);
		return 0;
	}

	while( ( ch = fgetc(ftmp) ) != EOF )
		fputc(ch, fp);

	fclose(ftmp);
	fclose(fp);
	return 1;
}


//display the list
void printList(bool active) {
	pathnode *ptr;
	DEBUG("Entry ");

	if(active)
		ptr = head;
	else
		ptr = temphead;

	//start from the beginning
	while(ptr != NULL) {
		DEBUG("%s",ptr->ref_path);
		ptr = ptr->next;
	}
}

void swap_heads() {
	DEBUG("Entry");
	pathnode *temp = head;
	head = temphead;
	temphead = temp;
}

// Insert link at the first location
void insert(char *data, bool active) {
	DEBUG("Entry |%s| active|%d|", data, active);
	//create a link
	pathnode *link = (pathnode*) calloc(1, sizeof(pathnode));
	if(!link) {
		ERR("Malloc failed!");
		return;
	}

	link->ref_path = data;

	if(active) {
		link->next = head;
		head = link;
	} else {
		link->next = temphead;
		temphead = link;
	}
}

// delete active list and swap with passive
void deleteList() {
	pathnode *ptr = head, *temp;
	DEBUG("Entry");
	if ( ptr == NULL) {
		DEBUG("List is empty");
	}
	//start from the beginning
	while(ptr != NULL) {
		temp = ptr;
		free(ptr->ref_path);
		if(ptr->next != NULL) {
			ptr = ptr->next;
		} else {
			ptr = NULL;
		}
		free(temp);
	}
	head = NULL;
	swap_heads();
}

enum {
	ABSOLUT_PATH =0,
	GLOB_FOUND,
	GLOB_MULTI_FOUND,
	REF_GLOB_FROUND
};

bool match(const char *string, const char *pattern)
{
	regex_t re;
	if (regcomp(&re, pattern, REG_EXTENDED) != 0) return 0;
	int status = regexec(&re, string, 0, NULL, 0);
	regfree(&re);
	if (status != 0) return false;
	return true;
}

//if matched start will have first match index, end will have end index
static bool is_res_required(char *str, int *start, int *len) {

	if (match(str, GLOB_CHAR)) {
		int s_len=0, b_len=0;
		char *star = strchr(str, '*');
		char *b_start = strchr(str, '[');
		char *b_end = strchr(str, ']');
		s_len = abs(star - str);
		b_len = abs(b_start - str);

		if(start==NULL)
			return true;

		*start = MIN(s_len, b_len);
		if (*start == s_len) {
			*len = 1;
		} else {
			*len = abs(b_end - b_start );
		}

		return true;
	}

	return false;
}

void add_data_blob(char *param, char *value, char *type)
{
	if(param==NULL || value==NULL || type==NULL)
		return;

	if (is_str_eq(type, "xsd:unsignedInt")) {
		blobmsg_add_u32(&bb, param, atoi(value));
	} else if (is_str_eq(type, "xsd:int")) {
		blobmsg_add_u32(&bb, param, atoi(value));
	} else if (is_str_eq(type, "xsd:long")) {
		blobmsg_add_double(&bb, param, atol(value));
	} else if (is_str_eq(type, "xsd:boolean")) {
		blobmsg_add_u8(&bb, param, atoi(value));
	} else { //"xsd:hexbin" "xsd:dateTime" "xsd:string"
		blobmsg_add_string(&bb, param, value);
	}
}


enum {
	DMPATH_NAME,
	__DM_MAX,
};

static const struct blobmsg_policy dm_get_policy[__DM_MAX] = {
	[DMPATH_NAME] = { .name = "path", .type = BLOBMSG_TYPE_STRING },
};

static bool cwmp_get(int operation, char *path, struct dmctx *dm_ctx) {
	int fault = 0;

	DEBUG("Entry |%s| operation|%d|", path, operation);
	switch(operation) {
		case CMD_GET_NAME:
			fault = dm_entry_param_method(dm_ctx, CMD_GET_NAME, path, "true", NULL);
			break;
		case CMD_GET_VALUE:
			fault = dm_entry_param_method(dm_ctx, CMD_GET_VALUE, path, NULL, NULL);
			break;
		default:
			ERR("Operation not supported yet!");
			return false;
	}

	if (dm_ctx->list_fault_param.next != &dm_ctx->list_fault_param) {
		struct param_fault *p;
		list_for_each_entry(p, &dm_ctx->list_fault_param, list) {
			blobmsg_add_u32(&bb, p->name, p->fault);
		}
		return false;
	}
	if (fault) {
		blobmsg_add_u32(&bb, "fault", fault);
		return false;
	}
	return true;
}

char *cwmp_get_value_by_id(char *id) {
	struct dmctx dm_ctx = {0};
	struct dm_parameter *n;
	char *value = NULL;

	DEBUG("Entry id |%s|", id);
	cwmp_init(&dm_ctx);
	if(cwmp_get(CMD_GET_VALUE, id, &dm_ctx)) {
			list_for_each_entry(n, &dm_ctx.list_parameter, list) {
				DEBUG("value |%s|", n->name);
				value = strdup(n->data); // mem will be freed on caller
				break;
			}
	}
	cwmp_cleanup(&dm_ctx);
	return (value);
}
static bool cwmp_set_value(char *path, char *value) {
	int fault = 0;
	struct dmctx dm_ctx = {0};
	struct dmctx *p_dmctx = &dm_ctx;
	void *bb_array = blobmsg_open_table(&bb, NULL);

	cwmp_init(&dm_ctx);
	DEBUG("Entry |%s|", path);
	fault = dm_entry_param_method(&dm_ctx, CMD_SET_VALUE, path, value, NULL);

	if(!fault) {
		fault = dm_entry_apply(&dm_ctx, CMD_SET_VALUE, "", NULL);
	}

	if (p_dmctx->list_fault_param.next != &p_dmctx->list_fault_param) {
		struct param_fault *p;
		list_for_each_entry(p, &p_dmctx->list_fault_param, list) {
			blobmsg_add_u32(&bb, p->name, p->fault);
		}
		return false;
	}

	if (fault) {
		blobmsg_add_u32(&bb, path, fault);
	} else {
		blobmsg_add_string(&bb, "path", path);
		blobmsg_add_u32(&bb, "status", 1);
	}

	blobmsg_close_table(&bb, bb_array);
	cwmp_cleanup(&dm_ctx);
	return true;
}

// return true ==> success
// false ==> failure
static bool cwmp_get_name(char *path) {
	struct dmctx dm_ctx = {0};
	struct dm_parameter *n;

	cwmp_init(&dm_ctx);
	if(cwmp_get(CMD_GET_NAME, path, &dm_ctx)) {
		list_for_each_entry(n, &dm_ctx.list_parameter, list) {
			DEBUG("get and insert |%s|", n->name);
			insert(strdup(n->name), false);
		}
	}
	cwmp_cleanup(&dm_ctx);
	return true;
}

static bool cwmp_get_value(char *path, bool fill) {
	struct dmctx dm_ctx = {0};
	struct dm_parameter *n;

	DEBUG("Entry path |%s|, fill|%d|", path, fill);
	cwmp_init(&dm_ctx);
	if(cwmp_get(CMD_GET_VALUE, path, &dm_ctx)) {
		if(fill) {
			void *bb_array = blobmsg_open_table(&bb, NULL);
			add_data_blob("path", path, "xsd:string");
			list_for_each_entry(n, &dm_ctx.list_parameter, list) {
				char *ref_path = NULL;
				ref_path = strrchr(n->name, '.');

				DEBUG("create blob with |%s|", n->name);
				add_data_blob(ref_path+1, n->data, n->type);
			}
			blobmsg_close_table(&bb, bb_array);
		}
	}
	cwmp_cleanup(&dm_ctx);
	return true;
}
bool cwmp_get_name_exp(char *path, char *operator, char *operand) {
	struct dmctx dm_ctx = {0};
	struct dm_parameter *n;
	bool ret = false;

	cwmp_init(&dm_ctx);
	if(cwmp_get(CMD_GET_VALUE, path, &dm_ctx)) {
		list_for_each_entry(n, &dm_ctx.list_parameter, list) {
			DEBUG("Get |%s| value|%s| operator|%s|", path, n->data, operator);
			int n1=0, n2=0;
			// fix this for all datatypes
			n1 = atoi(operand);
			n2 = atoi(n->data);
			if(is_str_eq("==", operator) && (0==strcasecmp(n->data, operand))) {
				ret = true;
			} else if(is_str_eq("!=",operator) && (0==strcasecmp(n->data, operand))) {
				ret = true;
			} else if(is_str_eq("<",operator)) {
				if ( n2 < n1 ) ret = true;
			} else if(is_str_eq(">",operator)) {
				if ( n2 > n1 ) ret = true;
			} else if(is_str_eq("<=",operator)) {
				if ( n2 <= n1 ) ret = true;
			} else if(is_str_eq(">=",operator)) {
				if ( n2 >= n1 ) ret = true;
			} else {
				ERR("Unknown operation");
			}
		}
	}

	cwmp_cleanup(&dm_ctx);
	return(ret);
}

void dereference_path(char *ref, char *l_op, char *r_op, char *op) {
	pathnode *p=head;

	while(p!=NULL) {
		char path[NAME_MAX]={'\0'};
		char ref_path[NAME_MAX]={'\0'};
		char *node = NULL;
		strcpy(path,p->ref_path);
		strcat(path, ref);
		node = cwmp_get_value_by_id(path);
		strcpy(ref_path, node);
		strcat(ref_path, l_op);
		DEBUG("de ref|%s|, path|%s|, node|%s|", ref_path, path, node);

		if(cwmp_get_name_exp(ref_path, op, r_op)) {
			insert(strdup(p->ref_path), false);
		}
		p=p->next;
	}
}
void tokenize(char *exp, char *l_op, char *r_op, char *op)
{
	bool operator_found = false;
	int index = 0, o_len=0;
	int len = strlen(exp);
	DEBUG("Entry exp|%s|", exp);
	for(int i=0; i<len; ++i) {
		switch(exp[i]) {
			case '"':
				{
					++i;
					while(exp[i]!='"') r_op[index++] = exp[i++];
					break;
				}
			case 'a' ... 'z':
			case 'A' ... 'Z':
			case '0' ... '9':
			case '.':
				if(operator_found) {
					r_op[index++]=exp[i];
				} else {
					l_op[index++]=exp[i];
				}
				break;
			case '=':
			case '!':
			case '>':
			case '<':
				operator_found = true;
				index=0;
				op[o_len++] = exp[i];
				break;
			default:
				ERR("Unknown symbol to parse [%c]", exp[i]);
				return;
		}
	}
}
// Optimize this function
void solve(char *exp) {
	DEBUG("Entry |%s|", exp);

	char operator[3]={'\0'};
	char token[NAME_MAX] = {'\0'};
	char operand[NAME_MAX] = {'\0'};
	char *plus = strchr(exp, '+');

	if(plus != NULL ) {
		char s[NAME_MAX] = {'\0'};
		strncpy(s, exp, plus-exp);
		tokenize(plus+2, token, operand, operator);
		dereference_path(s, token, operand, operator);
	} else {
		tokenize(exp, token, operand, operator);
		pathnode *p=head;
		while(p!=NULL) {
			char name[NAME_MAX]={'\0'};
			strcpy(name, p->ref_path);
			strcat(name, token);
			if(cwmp_get_name_exp(name, operator, operand)){
				insert(strdup(p->ref_path), false);
			}
			p=p->next;
		}
	}
	deleteList();
}

static int expand_expression(char *path, char *exp) {
	DEBUG("Entry path|%s|, exp|%s|", path, exp);
	int shiftpos;

	pathnode *p=head;
	while(p!=NULL) {
		cwmp_get_name(p->ref_path);
		p=p->next;
	}
	deleteList();

	switch(exp[0]) {
		case '*':
			shiftpos = 2;
			break;
		case '[':
			// Get multiple tokens and then evaluate
			shiftpos = strlen(exp) + 2;
			char *token;
			token = strtok(exp+1,"&&");
			/* walk through other tokens */
			while( token != NULL ) {
				DEBUG("solve %s", token );
				solve(token);
				token = strtok(NULL, "&&");
			}
			break;
		default:
			ERR("Unsupported case[%c]", path[0]);
			break;
	}
	return(shiftpos);
}
// return index and 
// int 
void filter_results(char *path, int start, int end) {
	int startpos = start, m_index=0, m_len=0;
	char *pp = path + startpos;
	char exp[NAME_MAX]={'\0'};
	DEBUG("Entry path|%s| start|%d| end|%d| pp|%s|", path, start, end, pp);

	if(start >= end) {
		return;
	}

	if(!is_res_required(pp, &m_index, &m_len)) {
		//append rest of the path to the final list
		if(pp == path ) {
			insert(strdup(pp), true);
			return;
		}

		pathnode *p=head;
		while(p!=NULL) {
			char name[NAME_MAX]={'\0'};
			strcpy(name, p->ref_path);
			strcat(name, pp);
			DEBUG("Final path[%s], ref |%s|", name, p->ref_path);
			insert(strdup(name), false);
			p = p->next;
		}
		deleteList();
		return;
	}

	// Get the string before the match
	char name[NAME_MAX]={'\0'};
	strncpy(name, pp, m_index);
	pathnode *p = head;

	if(p == NULL) {
		insert(strdup(name), false);
	}

	while(p!=NULL) {
		char ref_name[NAME_MAX]={'\0'};
		sprintf(ref_name, "%s%s", p->ref_path, name);
		insert(strdup(ref_name), false);
		p = p->next;
	}
	deleteList();
	startpos += m_index;

	strncpy(exp, pp+m_index, m_len);
	pp = path + startpos;
	int pos = 0;
	pos = expand_expression(pp, exp);
	startpos += pos;
	filter_results(path, startpos, end);
}

void create_response() {
	pathnode *p=head;

	DEBUG("Entry");
	while(p!=NULL) {
		cwmp_get_value(p->ref_path, true);
		p = p->next;
	}
}

static int get(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[__DM_MAX];
	char path[PATH_MAX] = {'\0'};
	blobmsg_parse(dm_get_policy, __DM_MAX, tb, blob_data(msg), blob_len(msg));

	if (!(tb[DMPATH_NAME]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	blob_buf_init(&bb, 0);
	blobmsg_buf_init(&bb);

	strcpy(path, blobmsg_data(tb[DMPATH_NAME]));
	DEBUG("Path |%s|",path);

	filter_results(path, 0, strlen(path));
	create_response();
	deleteList();

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

void operate_on_node(char *path) {
	DEBUG("Entry");
	int i=0, len=0;
	bool found = false;
	len = ARRAY_SIZE(operate_helper);
	for(i=0; i<len; i++) {
		if(match(path, operate_helper[i].name)) {
			operate_helper[i].opt(path);
			found = true;
			blobmsg_add_u32(&bb, "status", 1);
			break;
		}
	}
	if(!found) {
		blobmsg_add_u32(&bb, "status", 0);
		blobmsg_add_string(&bb, "error", "method not supported");
	}
}

static int operate(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[__DM_MAX];
	char path[PATH_MAX]={'\0'};
	char rel_path[PATH_MAX]={'\0'};
	char cmd[NAME_MAX]={'\0'};
	char *op_path = NULL;
	DEBUG("Entry");
	blobmsg_parse(dm_get_policy, __DM_MAX, tb, blob_data(msg), blob_len(msg));

	if (!(tb[DMPATH_NAME]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	blob_buf_init(&bb, 0);
	blobmsg_buf_init(&bb);


	strcpy(path, blobmsg_data(tb[DMPATH_NAME]));

	op_path = strrchr(path, '.');
	strcpy(cmd, op_path+1);

	strncpy(rel_path, path, op_path-path+1);
	filter_results(rel_path, 0, strlen(rel_path));

	pathnode *p=head;
	while(p!=NULL) {
		char tmp[NAME_MAX] = {'\0'};
		sprintf(tmp, "%s%s", p->ref_path, cmd);
		operate_on_node(tmp);
		p=p->next;
	}
	deleteList();

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

void opr_reboot_device(char *p) {
	DEBUG("entry");
	dmubus_call_set("system", "reboot", UBUS_ARGS{}, 0);
}

void opr_factory_reset(char *p) {
	DEBUG("Entry");
	DMCMD("/sbin/defaultreset", 0);
}

void opr_in_progress(char *p) {
	DEBUG("Not implemented!");
}

void opr_network_reset(char *p) {
	char *ret = strrchr(p, '.');
	char name[MAXNAMLEN] = {'\0'};
	char cmd[NAME_MAX] = "network.interface.";

	DEBUG("Entry path|%s|", p);

	strncpy(name, p, ret - p +1);
	strcat(name, "Name");
	char *zone = cwmp_get_value_by_id(name);
	if(zone) {
		strcat(cmd, zone);
		free(zone);
	} else {
		ERR("Network not rechable |%s|", cmd);
		return;
	}
	dmubus_call_set(cmd, "down", UBUS_ARGS{}, 0);
	dmubus_call_set(cmd, "up", UBUS_ARGS{}, 0);
}

enum {
	DM_SET_PATH,
	DM_SET_VALUE,
	__DMSET_MAX,
};
static const struct blobmsg_policy dm_set_policy[__DMSET_MAX] = {
	[DM_SET_PATH] = { .name = "path", .type = BLOBMSG_TYPE_STRING },
	[DM_SET_VALUE] = { .name = "value", .type = BLOBMSG_TYPE_STRING },
};

static int set_status(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[__DMSET_MAX];
	char path[PATH_MAX]={'\0'}, value[NAME_MAX]={'\0'};

	blobmsg_buf_init(&bb);
	blob_buf_init(&bb, 0);

	blobmsg_parse(dm_set_policy, __DMSET_MAX, tb, blob_data(msg), blob_len(msg));

	if (!(tb[DM_SET_PATH]) || !(tb[DM_SET_VALUE]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	strcpy(path, blobmsg_data(tb[DM_SET_PATH]));
	strcpy(value, blobmsg_data(tb[DM_SET_VALUE]));
	DEBUG("set path |%s| value |%s|",path, value);

	filter_results(path, 0, strlen(path));

	pathnode *p=head;
	while(p!=NULL) {
		cwmp_set_value(p->ref_path, value);
		p=p->next;
	}
	deleteList();

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}


struct ubus_method usp_methods[] = {
	UBUS_METHOD("get", get, dm_get_policy),
	UBUS_METHOD("set", set_status, dm_set_policy),
	UBUS_METHOD("operate", operate, dm_get_policy),
};

struct ubus_object_type usp_type =
	UBUS_OBJECT_TYPE("usp", usp_methods);

struct ubus_object usp_object = {
	.name = "usp",
	.type = &usp_type,
	.methods = usp_methods,
	.n_methods = ARRAY_SIZE(usp_methods),
};
