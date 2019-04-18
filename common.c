/*
 * common.c: Common utils of Get/Set/Operate handlers
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

#include "common.h"

#define GLOB_CHAR "[[+*#]+"
#define DELIM '.'
#define RESULT_STACK 15

static bool is_node_instance(char *path);
static bool is_leaf(char *path, char *);
static void process_result(struct blob_buf *bb, unsigned int len);
// Global variables
typedef struct resultnode {
	char *name;
	char *value;
	char *type;
	struct resultnode *next;
} resultnode;

typedef struct resultstack {
	void *cookie;
	char *key;
} resultstack;

static int rtop=-1;

static resultstack g_result[RESULT_STACK];
static resultnode *rnode = NULL;
pathnode *head = NULL;
static pathnode *temphead = NULL;

// Stack utils
bool is_stack_empty() {
	return (rtop == -1);
}

bool push() {
	DEBUG("entry");
	if(rtop >= RESULT_STACK) {
		ERR("Stack full, change stack logic");
		return false;
	}
	rtop ++;
	return true;
}

bool pop() {
	DEBUG("entry");
	if(is_stack_empty()) {
		INFO("Stack empty, can't pop");
		return false;
	}
	free(g_result[rtop].key);
	memset(&g_result[rtop], '\0', sizeof(resultstack));
	rtop--;
	return true;
}

bool top() {
	if(is_stack_empty()) {
		INFO("Stack empty, no top");
		return false;
	}
	return true;
}

// Common utilities
bool is_str_eq(char *s1, char *s2) {
	if(0==strcmp(s1, s2))
		return true;

	return false;
}

static bool is_node_instance(char *path) {
	int ins = -1;

	ins = atoi(path);
	return(ins != 0);
}

// RE utilities
bool match(const char *string, const char *pattern) {
	regex_t re;
	if (regcomp(&re, pattern, REG_EXTENDED) != 0) return 0;
	int status = regexec(&re, string, 0, NULL, 0);
	regfree(&re);
	if (status != 0) return false;
	return true;
}

static void cwmp_init(struct dmctx *dm_ctx) {
	dm_ctx_init(dm_ctx, DM_CWMP, AMD_2, INSTANCE_MODE_NUMBER);
}

static void cwmp_cleanup(struct dmctx *dm_ctx) {
	dm_ctx_clean(dm_ctx);
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
	*start = strlen(str);
	return false;
}

static void add_data_blob(struct blob_buf *bb, char *param, char *value, char *type) {
	if(param==NULL || value==NULL || type==NULL)
		return;

	if (is_str_eq(type, "xsd:unsignedInt")) {
		blobmsg_add_u32(bb, param, atoi(value));
	} else if (is_str_eq(type, "xsd:int")) {
		blobmsg_add_u32(bb, param, atoi(value));
	} else if (is_str_eq(type, "xsd:long")) {
		blobmsg_add_double(bb, param, atol(value));
	} else if (is_str_eq(type, "xsd:boolean")) {
		blobmsg_add_u8(bb, param, atoi(value));
	} else { //"xsd:hexbin" "xsd:dateTime" "xsd:string"
		blobmsg_add_string(bb, param, value);
	}
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

static void swap_heads() {
	DEBUG("Entry");
	pathnode *temp = head;
	head = temphead;
	temphead = temp;
}


// Insert link at the first location
static void insert_result(char *name, char *value, char *type) {
	DEBUG("Entry result |%s| value|%d|", name, value);
	//create a link
	resultnode *link = (resultnode*) calloc(1, sizeof(resultnode));
	if(!link) {
		ERR("Malloc failed!");
		return;
	}

	link->name = strdup(name);
	link->value = strdup(value);
	link->type = strdup(type);

	link->next = rnode;
	rnode = link;
}

static void delete_result() {
	resultnode *ptr = rnode, *temp;
	DEBUG("Entry");
	if ( ptr == NULL) {
		INFO("Result List is empty");
	}
	//start from the beginning
	while(ptr != NULL) {
		temp = ptr;
		free(ptr->name);
		free(ptr->value);
		free(ptr->type);

		if(ptr->next != NULL) {
			ptr = ptr->next;
		} else {
			ptr = NULL;
		}
		free(temp);
	}
	rnode = NULL;
}

static void rev_result() {
	resultnode *tmp = rnode;
	rnode = NULL;
	while(tmp != NULL) {
		resultnode *t = tmp;
		insert_result(tmp->name, tmp->value, tmp->type);
		free(tmp->name);
		free(tmp->value);
		free(tmp->type);
		tmp = tmp->next;
		free(t);
	}
}

// Insert link at the first location
static void insert(char *data, bool active) {
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
		return false;
	}
	if (fault) {
		return false;
	}
	return true;
}

// return true ==> success
// false ==> failure
bool cwmp_get_name(char *path) {
	struct dmctx dm_ctx = {0};
	struct dm_parameter *n;
	bool ret = false;

	cwmp_init(&dm_ctx);
	if(cwmp_get(CMD_GET_NAME, path, &dm_ctx)) {
		list_for_each_entry(n, &dm_ctx.list_parameter, list) {
			DEBUG("get and insert |%s|", n->name);
			insert(strdup(n->name), false);
		}
		ret = true;
	}
	cwmp_cleanup(&dm_ctx);
	return(ret);
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

// path = 1.Stats.BytesReceived
static bool leaf_same_group(char *path) {
	DEBUG("entry");
	char *key;

	if(top()) {
		key = g_result[rtop].key;
		unsigned int klen = strlen(key);

		if(0 == strncmp(path, key, klen)) {
			DEBUG("same group|%s| path|%s|", g_result[rtop].key, path);
			return true;
		}
	}
	return false;
}

static bool is_leaf(char *path, char *node) {
	DEBUG("entry path|%s|", path);
	char *ret = NULL;
	ret =  strchr(path, DELIM);

	if(ret != NULL) {
		strncpy(node, path, strlen(path) - strlen(ret)+1);
	}

	return (ret==NULL);
}

static void process_result(struct blob_buf *bb, unsigned int len) {
	char pn[NAME_MAX]={'\0'};

	if(rnode == NULL) {
		return;
	}

	DEBUG("Entry node |%s|, len|%d|", rnode->name, len);

	if(leaf_same_group(rnode->name)) {
		if(is_leaf(rnode->name+len, pn)) {
			//INFO("add leaf |%s|", rnode->name+len);
			add_data_blob(bb, rnode->name+len, rnode->value, rnode->type);
		} else {
			//check after stack top it's a leaf
			if(top()) {
				if(is_str_eq(g_result[rtop].key, pn)) {
					len = strlen(g_result[rtop].key);
				} else {
					push();
					char temp[NAME_MAX] = {'\0'};
					strncpy(temp, rnode->name, len);
					strcat(temp, pn);
					//INFO("Push1 |%s|, node|%s|", temp, pn);
					len = strlen(temp);

					char table_name[NAME_MAX]={'\0'};
					strncpy(table_name, pn, strlen(pn)-1);

					if(is_node_instance(rnode->name + len)) {
						//INFO("Open table |%s|", table_name);
						g_result[rtop].cookie = blobmsg_open_array(bb, table_name);
						g_result[rtop].key = strdup(temp);
					} else {
						if(is_node_instance(pn)) {
							g_result[rtop].cookie = blobmsg_open_table(bb, NULL);
						} else {
							g_result[rtop].cookie = blobmsg_open_table(bb, table_name);
						}
						g_result[rtop].key = strdup(temp);
					}
				}

			} else {
				push();
				char temp[NAME_MAX] = {'\0'};
				strncpy(temp, rnode->name, len);
				strcat(temp, pn);
				//INFO("Push2 |%s|, node|%s|", temp, pn);
				len = strlen(temp);

				char table_name[NAME_MAX]={'\0'};
				strncpy(table_name, pn, strlen(pn)-1);

				if(is_node_instance(rnode->name + len)) {
					//INFO("Open table |%s|", table_name);
					g_result[rtop].cookie = blobmsg_open_array(bb, table_name);
					g_result[rtop].key = strdup(temp);
				} else {
					if(is_node_instance(pn)) {
						g_result[rtop].cookie = blobmsg_open_table(bb, NULL);
					} else {
						g_result[rtop].cookie = blobmsg_open_table(bb, table_name);
					}
					g_result[rtop].key = strdup(temp);
				}
			}
			process_result(bb, len);
		}
	} else {
		// check if it still belong to the group on stack
		if(top()) { // if element present in stack but not matching
			//INFO("Closing table for |%s|", g_result[rtop].key);
			blobmsg_close_table(bb,g_result[rtop].cookie);
		}
		if(is_leaf(rnode->name, pn)) {
			//INFO("add in blob|%s|, value|%s|, type|%s|", rnode->name, rnode->value, rnode->type);
			add_data_blob(bb, rnode->name, rnode->value, rnode->type);
		} else {
			if(top()) { // if element present in stack but not matching
				pop();
				if(top())
					len = strlen(g_result[rtop].key);
			} else {
				if(push() == false)
					return;
				//INFO("Pushing |%s|", pn);
				len = strlen(pn);

				char table_name[NAME_MAX]={'\0'};
				strncpy(table_name, pn, len-1);

				if(is_node_instance(rnode->name + len)) {
					//INFO("Open table |%s|", table_name);
					g_result[rtop].cookie = blobmsg_open_array(bb, table_name);
					g_result[rtop].key = strdup(pn);
				} else {
					if(is_node_instance(pn)) {
						g_result[rtop].cookie = blobmsg_open_table(bb, NULL);
					} else {
						g_result[rtop].cookie = blobmsg_open_table(bb, table_name);
					}
					g_result[rtop].key = strdup(pn);
				}
			}
			process_result(bb, len);
		}
	}
	if(rnode != NULL)
		rnode = rnode->next;

	process_result(bb, len);
}


static int get_glob_len(char *path) {
	int m_index = 0, m_len=0, ret=0;
	int plen = strlen(path);
	DEBUG("Entry");
	if(is_res_required(path, &m_index, &m_len)) {
		char temp_name[NAME_MAX] = {'\0'};
		strncpy(temp_name, path, m_index - 1 );
		char *end = strrchr(temp_name, DELIM);
		ret = m_index - strlen(end);
	} else {
		char name[NAME_MAX] = {'\0'};
		if(path[plen - 1] == DELIM) {
			strncpy(name, path, plen -1 );
		} else {
			ret = 1;
			strncpy(name, path, plen );
		}
		char *end = strrchr(name, DELIM);
		ret = ret + strlen(path) - strlen(end);
		if(is_node_instance(end+1)) {
			char temp_name[NAME_MAX] = {'\0'};
			strncpy(temp_name, path, plen - strlen(end) - 1 );
			end = strrchr(temp_name, DELIM);
			ret = ret - strlen(end);
		}
	}
	return(ret);
}

bool cwmp_get_value(char *path, bool fill, char *query_path) {
	struct dmctx dm_ctx = {0};
	struct dm_parameter *n;
	int plen = get_glob_len(query_path);

	DEBUG("Entry path |%s|, fill|%d|, query_path|%s|", path, fill, query_path);

	cwmp_init(&dm_ctx);
	if(cwmp_get(CMD_GET_VALUE, path, &dm_ctx)) {
		if(fill) {
			list_for_each_entry(n, &dm_ctx.list_parameter, list) {
				DEBUG("insert node|%s|, value|%s| ", n->name+plen, n->data);
				insert_result(n->name+plen, n->data, n->type);
			}
		}
	}
	cwmp_cleanup(&dm_ctx);
	return true;
}

void prepare_result(struct blob_buf *bb) {
	rev_result();
	resultnode *rhead = rnode;
	process_result(bb, 0);
	while(top()){
		DEBUG("Close all open tables");
		blobmsg_close_table(bb,g_result[rtop].cookie);
		pop();
	}
	rnode = rhead;
	delete_result();
}

static bool cwmp_get_name_exp(char *path, char *operator, char *operand) {
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

bool cwmp_set_value(struct blob_buf *bb, char *path, char *value) {
	int fault = 0;
	struct dmctx dm_ctx = {0};
	struct dmctx *p_dmctx = &dm_ctx;
	void *bb_array = blobmsg_open_table(bb, NULL);

	cwmp_init(&dm_ctx);
	DEBUG("Entry |%s|", path);
	fault = dm_entry_param_method(&dm_ctx, CMD_SET_VALUE, path, value, NULL);

	if(!fault) {
		fault = dm_entry_apply(&dm_ctx, CMD_SET_VALUE, "", NULL);
	}

	if (p_dmctx->list_fault_param.next != &p_dmctx->list_fault_param) {
		struct param_fault *p;
		list_for_each_entry(p, &p_dmctx->list_fault_param, list) {
			blobmsg_add_u32(bb, p->name, p->fault);
		}
		return false;
	}

	if (fault) {
		blobmsg_add_u32(bb, path, fault);
	} else {
		blobmsg_add_string(bb, "path", path);
		blobmsg_add_u32(bb, "status", 1);
	}

	blobmsg_close_table(bb, bb_array);
	cwmp_cleanup(&dm_ctx);
	return true;
}

static void dereference_path(char *ref, char *l_op, char *r_op, char *op) {
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
		free(node);

		if(cwmp_get_name_exp(ref_path, op, r_op)) {
			insert(strdup(p->ref_path), false);
		}
		p=p->next;
	}
}

static void tokenize(char *exp, char *l_op, char *r_op, char *op) {
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

static void solve(char *exp) {
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

