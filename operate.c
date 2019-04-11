/*
 * operate.c: Operate handler for uspd
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
#include "operate.h"

extern pathnode *head;
// Operate function definations
static void opr_reboot_device(char *p) {
	DEBUG("entry");
	dmubus_call_set("system", "reboot", UBUS_ARGS{}, 0);
}

static void opr_factory_reset(char *p) {
	DEBUG("Entry");
	dmcmd_no_wait("/sbin/defaultreset", 0);
}

static void opr_in_progress(char *p) {
	DEBUG("Not implemented!");
}

static void opr_network_reset(char *p) {
	char *ret = strrchr(p, '.');
	char name[MAXNAMLEN] = {'\0'};
	char cmd[NAME_MAX] = "network.interface.";

	DEBUG("Entry path|%s|", p);

	strncpy(name, p, ret - p +1);
	strcat(name, "Name");
	char *zone = NULL;
	zone = cwmp_get_value_by_id(name);
	if(zone) {
		strcat(cmd, zone);
		free(zone);
	} else {
		ERR("Network not rechable |%s|", cmd);
		return;
	}
	DEBUG("cmd |%s|", cmd);
	dmubus_call_set(cmd, "down", UBUS_ARGS{}, 0);
	dmubus_call_set(cmd, "up", UBUS_ARGS{}, 0);
}

static struct op_cmd operate_helper[] = {
	[0] = {"Device.Reboot()", opr_reboot_device},
	[1] = {"Device.FactoryReset()", opr_factory_reset},
	[2] = {"Device.IP.Interface.*.Reset()", opr_network_reset}
};

static void operate_on_node(struct blob_buf *bb, char *path) {
	DEBUG("Entry");
	int i=0, len=0;
	bool found = false;
	len = ARRAY_SIZE(operate_helper);
	for(i=0; i<len; i++) {
		if(match(path, operate_helper[i].name)) {
			operate_helper[i].opt(path);
			found = true;
			blobmsg_add_u32(bb, "status", 1);
			break;
		}
	}
	if(!found) {
		blobmsg_add_u32(bb, "status", 0);
		blobmsg_add_string(bb, "error", "method not supported");
	}
}

void create_operate_response(struct blob_buf *bb, char *cmd) {

	pathnode *p=head;
	while(p!=NULL) {
		char tmp[NAME_MAX] = {'\0'};
		sprintf(tmp, "%s%s", p->ref_path, cmd);
		operate_on_node(bb, tmp);
		p=p->next;
	}
	deleteList();
}

