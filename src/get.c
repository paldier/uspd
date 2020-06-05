/*
 * get.c: Get handler for uspd
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
#include "get.h"
#include "strncpyt.h"

extern pathnode *head;

void create_response(struct blob_buf *bb, char *qpath, uint8_t maxdepth) {
	pathnode *p=head;
	p = head;
	while(p!=NULL) {
		if (bbf_get_value(p->ref_path, true, qpath, bb, maxdepth))
			break;
		p = p->next;
	}
	deleteList();
	prepare_result(bb);
}

void create_raw_response(struct blob_buf *bb) {
	pathnode *p=head;
	void *array = blobmsg_open_array(bb, "parameters");
	while(p!=NULL) {
		if (bbf_get_value_raw(p->ref_path, bb))
			break;
		p = p->next;
	}
	blobmsg_close_array(bb, array);
	deleteList();
	prepare_result(bb);
}

void get_resolved_path(struct blob_buf *bb)
{
	pathnode *p=head;
	void *array, *table;

	array = blobmsg_open_array(bb, "parameters");
	while(p != NULL) {
		char temp[NAME_MAX] = { };
		size_t nlen = strlen(p->ref_path);
		strncpyt(temp, p->ref_path, nlen);
		if (temp[nlen - 1] == '.')
			temp[nlen - 1] = '\0';

		table = blobmsg_open_table(bb, NULL);

		blobmsg_add_string(bb, "parameter", temp);
		blobmsg_close_table(bb, table);
		p = p->next;
	}
	blobmsg_close_array(bb, array);
	deleteList();
}

void create_inst_name_response(struct blob_buf *bb) {
	pathnode *p=head;
	void *array = blobmsg_open_array(bb, "parameters");
	while(p!=NULL) {
		if (bbf_get_inst_name_raw(p->ref_path, bb))
			break;
		p = p->next;
	}
	blobmsg_close_array(bb, array);
	deleteList();
}

void create_name_response(struct blob_buf *bb) {
	pathnode *p=head;
	void *array = blobmsg_open_array(bb, "parameters");
	while(p!=NULL) {
		if (bbf_get_name_raw(p->ref_path, bb))
			break;
		p = p->next;
	}
	blobmsg_close_array(bb, array);
	deleteList();
}

