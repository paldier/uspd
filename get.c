/*
 * get.c: Get handler for uspd
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
#include "get.h"
extern pathnode *head;

void create_response(struct blob_buf *bb) {
	pathnode *p=head;
	bool names_present = false;
	printList(true);
	DEBUG("Entry");
	// First get the names
	while(p!=NULL) {
		if(false == cwmp_get_name(p->ref_path)) {
			names_present = false;
			break;
		}
		names_present = true;
		p = p->next;
	}
	if(names_present)
		deleteList();

	p = head;
	while(p!=NULL) {
		cwmp_get_value(bb, p->ref_path, true);
		p = p->next;
	}
	deleteList();
}

