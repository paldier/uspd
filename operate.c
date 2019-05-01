/*
 * operate.c: Operate handler for uspd
 *
 * Copyright (C) 2019 iopsys Software Solutions AB. All rights reserved.
 *
 * Author: Vivek Dutta <v.dutta@gxgroup.eu>
 * Author: Yashvardhan <y.yashvardhan@gxgroup.eu>
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
static void reboot_device(char *p) {
	DEBUG("entry");
	dmubus_call_set(SYSTEM_UBUS_PATH, "reboot", UBUS_ARGS{}, 0);
}

static void factory_reset(char *p) {
	DEBUG("Entry");
	dmcmd_no_wait("/sbin/defaultreset", 0);
}

static void network_reset(char *p) {
	char *ret = strrchr(p, '.');
	char name[MAXNAMLEN] = {'\0'};
	char cmd[NAME_MAX] = NETWORK_INTERFACE_UBUS_PATH;

	DEBUG("Entry path|%s|", p);

	snprintf(cmd + strlen(cmd), NAME_MAX - strlen(cmd), "%s", ".");
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

static void wireless_reset(char *p)
{
	dmcmd_no_wait("/sbin/wifi", 2, "reload", "&");
}

static void dhcpv4_client_renew(char *p)
{
	struct blob_buf blob;
	memset(&blob, 0, sizeof(blob));
	blob_buf_init(&blob, 0);

	cwmp_set_value(&blob, p, "true");
}

static struct op_cmd operate_helper[] = {
	[0] = {"Device.Reboot", reboot_device},
	[1] = {"Device.FactoryReset", factory_reset},
	[2] = {"Device.IP.Interface.*.Reset", network_reset},
	[3] = {"Device.WiFi.Reset", wireless_reset},
	[4] = {"Device.DHCPv4.Client.*.Renew", dhcpv4_client_renew}
};

static void operate_on_node(struct blob_buf *bb, char *path) {
	DEBUG("Entry path|%s|", path);
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

