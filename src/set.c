/*
 * set.c: Set handler for uspd
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
#include "set.h"

extern pathnode *head;

void create_set_response(struct blob_buf *bb, char *value, char *key) {
	pathnode *p=head;
	while(p!=NULL) {
		if (bbf_set_value(bb, p->ref_path, value, key))
			break;
		p=p->next;
	}
	deleteList();
}

void set_multiple_values(struct blob_buf *bb, struct blob_attr *blob_value, char *key) {

	size_t tlen = (size_t)blobmsg_data_len(blob_value);
	struct blob_attr *attr;

	__blob_for_each_attr(attr, blobmsg_data(blob_value), tlen) {
		struct blobmsg_hdr *hdr = blob_data(attr);

		pathnode *p=head;
		while(p!=NULL) {
			char path[PATH_MAX] = {'\0'}, value[NAME_MAX] = {'\0'};
			snprintf(path, PATH_MAX, "%s%s", p->ref_path, hdr->name);

			switch(blob_id(attr)) {
				case BLOBMSG_TYPE_STRING:
					snprintf(value, NAME_MAX, "%s", (char *)blobmsg_data(attr));
					break;
				case BLOBMSG_TYPE_INT8:
					sprintf(value, "%d", *(uint8_t *)blobmsg_data(attr));
					break;
				case BLOBMSG_TYPE_INT16:
					sprintf(value, "%d", *(uint16_t *)blobmsg_data(attr));
					break;
				case BLOBMSG_TYPE_INT32:
					sprintf(value, "%u", *(uint32_t *)blobmsg_data(attr));
					break;
				case BLOBMSG_TYPE_INT64:
				case BLOBMSG_TYPE_DOUBLE:
					snprintf(value, NAME_MAX, "%"PRIu64"", *(uint64_t *)blobmsg_data(attr));
					break;
				default:
					INFO("Unhandled set request type|%x|", blob_id(attr));
					break;
			}

			if (bbf_set_value(bb, path, value, key))
				break;
			p=p->next;
		}
	}
	deleteList();
}
