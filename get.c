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
extern pathnode *head;

void create_response(struct blob_buf *bb, char *qpath) {
	pathnode *p=head;
	p = head;
	while(p!=NULL) {
		bbf_get_value(p->ref_path, true, qpath);
		p = p->next;
	}
	deleteList();
	prepare_result(bb);
}

void create_raw_response(struct blob_buf *bb) {
	pathnode *p=head;
	void *array = blobmsg_open_array(bb, "parameters");
	while(p!=NULL) {
		bbf_get_value_raw(p->ref_path, bb);
		p = p->next;
	}
	blobmsg_close_array(bb, array);
	deleteList();
	prepare_result(bb);
}

