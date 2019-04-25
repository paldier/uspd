/*
 * set.c: Set handler for uspd
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
#include "set.h"

extern pathnode *head;

void create_set_response(struct blob_buf *bb, char *value) {
	pathnode *p=head;
	while(p!=NULL) {
		cwmp_set_value(bb, p->ref_path, value);
		p=p->next;
	}
	deleteList();
}

void set_multiple_values(struct blob_buf *bb, struct blob_attr *blob_value) {

	int tlen = blobmsg_data_len(blob_value);
	struct blob_attr *attr;

	__blob_for_each_attr(attr, blobmsg_data(blob_value), tlen) {
		struct blobmsg_hdr *hdr = blob_data(attr);

		pathnode *p=head;
		while(p!=NULL) {
			char path[PATH_MAX] = {'\0'}, value[NAME_MAX] = {'\0'};
			sprintf(path, "%s%s", p->ref_path, hdr->name);

			switch(blob_id(attr)) {
				case BLOBMSG_TYPE_STRING:
					sprintf(value, "%s", (char *)blobmsg_data(attr));
					break;
				case BLOBMSG_TYPE_INT8:
					sprintf(value, "%d", *(uint8_t *)blobmsg_data(attr));
					break;
				case BLOBMSG_TYPE_INT16:
					sprintf(value, "%d", *(uint16_t *)blobmsg_data(attr));
					break;
				case BLOBMSG_TYPE_INT32:
					sprintf(value, "%d", *(uint32_t *)blobmsg_data(attr));
					break;
				case BLOBMSG_TYPE_INT64:
				case BLOBMSG_TYPE_DOUBLE:
					sprintf(value, "%lld", *(uint64_t *)blobmsg_data(attr));
					break;
				default:
					INFO("Unhandled set request type|%d|", blob_id(attr));
					break;
			}

			cwmp_set_value(bb, path, value);
			p=p->next;
		}
	}
	deleteList();
}
