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

// Operate function definitions
static opr_ret_t reboot_device(struct blob_buf *bb, uint8_t *p, struct blob_attr
			       *bv)
{
	DEBUG("entry |%s| |%s|", p, bv->data);

	if(0 == dmubus_call_set(SYSTEM_UBUS_PATH, "reboot", UBUS_ARGS{}, 0))
		blobmsg_add_u32(bb, "status", 1);
	else
		blobmsg_add_u32(bb, "status", 0);

	return SUCCESS;
}

static opr_ret_t factory_reset(struct blob_buf *bb, uint8_t *p, struct blob_attr
			       *bv)
{
	DEBUG("entry |%s| |%s|", p, bv->data);

	if(0 == dmcmd_no_wait("/sbin/defaultreset", 0))
		blobmsg_add_u32(bb, "status", 1);
	else
		blobmsg_add_u32(bb, "status", 0);
	return SUCCESS;
}

static char *get_param_val_from_op_cmd(char *op_cmd, const char *param)
{
	char *val = NULL;
	char node[MAXNAMLEN] = {'\0'};

	// Trim action from operation command
	// For eg: trim Reset from Device.IP.Interface.*.Reset
	char *ret = strrchr(op_cmd, '.');
	strncpy(node, op_cmd, ret - op_cmd +1);

	// Append param name to the trimmed path
	strcat(node, param);

	// Get parameter value
	val = cwmp_get_value_by_id(node);
	return(val);
}

static opr_ret_t network_interface_reset(struct blob_buf *bb, char *p, struct
					 blob_attr *bv)
{
	char cmd[NAME_MAX] = NETWORK_INTERFACE_UBUS_PATH;
	bool status = false;

	DEBUG("entry |%s| |%s|", p, bv->data);

	snprintf(cmd + strlen(cmd), NAME_MAX - strlen(cmd), "%s", ".");
	char *zone = NULL;
	zone = get_param_val_from_op_cmd(p, "Name");
	if(zone) {
		strcat(cmd, zone);
		free(zone);
	} else {
		ERR("Network not reachable |%s|", cmd);
		return FAIL;
	}
	DEBUG("cmd |%s|", cmd);
	if(0 == dmubus_call_set(cmd, "down", UBUS_ARGS{}, 0))
		status = true;

	if(0 == dmubus_call_set(cmd, "up", UBUS_ARGS{}, 0))
		status &= true;

	blobmsg_add_u32(bb, "status", status);
	return SUCCESS;
}

static opr_ret_t wireless_reset(struct blob_buf *bb, char *p, struct blob_attr
				*bv)
{
	DEBUG("entry |%s| |%s|", p, bv->data);

	if(0 == dmcmd_no_wait("/sbin/wifi", 2, "reload", "&"))
		blobmsg_add_u32(bb, "status", 1);
	else
		blobmsg_add_u32(bb, "status", 0);
	return SUCCESS;
}

struct wifi_security_params reset_params[] = {
	{"", "ModeEnabled", ""},
	{"", "PreSharedKey", ""},
	{"", "KeyPassphrase", ""}
};

static opr_ret_t ap_security_reset(struct blob_buf *bb, char *p, struct
					 blob_attr *bv)
{
	char *wpakey = NULL;
	char node[MAXNAMLEN] = {'\0'};
	int i, len = 0;

	char *ret = strrchr(p, '.');
	strncpy(node, p, ret - p +1);

	len = ARRAY_SIZE(reset_params);

	for (i = 0; i < len; i++) {
		strncpy(reset_params[i].node, node, MAXNAMLEN);
		strcat(reset_params[i].node, reset_params[i].param);
		DEBUG("Node|%s|", reset_params[i].node);
	}
	const char *mode_enabled = "WPA2-Personal";

	// Default mode - WPA2-Personal
	strncpy(reset_params[0].value, mode_enabled, MAXNAMLEN);

	// Get Default wpakey
	db_get_value("hw", "board", "wpaKey", &wpakey);

	// PreSharedKey and KeyPassphrase are kept same
	strncpy(reset_params[1].value, wpakey, MAXNAMLEN);
	strncpy(reset_params[2].value, wpakey, MAXNAMLEN);

	for (i = 0; i < len; i++) {
		cwmp_set_value(bb, reset_params[i].node, reset_params[i].value);
	}
	return SUCCESS;
}

static opr_ret_t dhcp_client_renew(struct blob_buf *bb, char *p, struct
				   blob_attr *bv)
{
	DEBUG("entry |%s| |%s|", p, bv->data);

	cwmp_set_value(bb, p, "true");
	return SUCCESS;
}

static bool get_value_from_blob(struct blob_attr *bv, char *key, char *value)
{
	struct blob_attr *attr;
	char tlen = blobmsg_data_len(bv);

	__blob_for_each_attr(attr, blobmsg_data(bv), tlen) {
		struct blobmsg_hdr *hdr = blob_data(attr);
		DEBUG("key|%s|", hdr->name);

		if(is_str_eq((char *)hdr->name, key)) {
			switch(blob_id(attr)) {
				case BLOBMSG_TYPE_STRING:
					{
						strcpy(value, (char *)blobmsg_data(attr));
					}
					break;
				case BLOBMSG_TYPE_INT8:
					{
						sprintf(value, "%d", *(uint8_t *)blobmsg_data(attr));
					}
					break;
				case BLOBMSG_TYPE_INT16:
					{
						sprintf(value, "%d", *(uint8_t *)blobmsg_data(attr));
					}
					break;
				case BLOBMSG_TYPE_INT32:
					{
						sprintf(value, "%d", *(uint8_t *)blobmsg_data(attr));
					}
					break;
				case BLOBMSG_TYPE_INT64:
				case BLOBMSG_TYPE_DOUBLE:
					{
						sprintf(value, "%lld", *(uint64_t *)blobmsg_data(attr));
					}
					break;
				default:
					INFO("Unhandled blob type|%d|", blob_id(attr));
					break;
			}
			return true;
		}
	}
	return false;
}

static void fill_wireless_scan_results(struct blob_buf *bb, char *radio) {
	json_object *res, *obj;

	dmubus_call(ROUTER_WIRELESS_UBUS_PATH, "scanresults",
		    UBUS_ARGS{{"radio", radio, String}}, 1, &res);

	if(!json_object_object_get_ex(res,"access_points", &obj)) {
		ERR("access_point not found in results");
		json_object_put(res);
		return;
	}

	uint8_t len = json_object_array_length(obj);
	for (uint8_t j = 0; j < len; j++ ) {
		void *table = blobmsg_open_table(bb, NULL);
		json_object *array_obj = json_object_array_get_idx(obj, j);
		if(!blobmsg_add_object(bb, array_obj))
			ERR("Failed to add reply in bb");
		blobmsg_close_table(bb, table);
	}
	json_object_put(obj);
	json_object_put(res);
}

static opr_ret_t fetch_neighboring_wifi_diagnostic(struct blob_buf *bb, char
						   *path, struct blob_attr *bv)
{
	struct dmctx dm_ctx = {0};
	json_object *res;
	cwmp_init(&dm_ctx, path);

	DEBUG("entry |%s| |%s|", path, bv->data);

	void *array = blobmsg_open_array(bb, "Result.");

	dmubus_call(ROUTER_WIRELESS_UBUS_PATH, "radios", UBUS_ARGS{{}}, 0, &res);
	json_object_object_foreach(res, key, val) {
		fill_wireless_scan_results(bb, key);
	}

	blobmsg_close_table(bb, array);
	cwmp_cleanup(&dm_ctx);
	json_object_put(res);
	return SUCCESS;
}

static opr_ret_t vendor_conf_backup(struct blob_buf *bb, char *path, struct
				    blob_attr *bv)
{
	struct file_server fserver = {0};
	char *vcf_name = NULL;

	vcf_name = get_param_val_from_op_cmd(path, "Name");
	if (!vcf_name)
		return FAIL;

	if(!get_value_from_blob(bv, "URL", fserver.url))
		return UBUS_INVALID_ARGUMENTS;

	if(!get_value_from_blob(bv, "Username", fserver.user))
		return UBUS_INVALID_ARGUMENTS;

	if(!get_value_from_blob(bv, "Password", fserver.pass))
		return UBUS_INVALID_ARGUMENTS;

	DEBUG("url|%s|, user|%s|, pass|%s|", fserver.url, fserver.user,
	     fserver.pass);

	dmcmd("/bin/sh", 7, ICWMP_SCRIPT, "upload", fserver.url,
		   VCF_FILE_TYPE, fserver.user, fserver.pass, vcf_name);
	// TODO Add error handling

	blobmsg_add_u32(bb, "status", 1);
	return SUCCESS;
}

static opr_ret_t vendor_conf_restore(struct blob_buf *bb, char *path, struct
				     blob_attr *bv)
{
	struct file_server fserver = {0};
	char file_size[128];

	DEBUG("entry |%s| |%s|", path, bv->data);

	if (!get_value_from_blob(bv, "URL", fserver.url))
		return UBUS_INVALID_ARGUMENTS;

	if (!get_value_from_blob(bv, "Username", fserver.user))
		return UBUS_INVALID_ARGUMENTS;

	if (!get_value_from_blob(bv, "Password", fserver.pass))
		return UBUS_INVALID_ARGUMENTS;

	get_value_from_blob(bv, "FileSize", file_size);

	DEBUG("url|%s|, user|%s|, pass|%s|", fserver.url,
	     fserver.user, fserver.pass);

	dmcmd("/bin/sh", 7, ICWMP_SCRIPT, "download", fserver.url,
		   file_size, VCF_FILE_TYPE, fserver.user,
		   fserver.pass);
	// TODO Add error handling

	if (0 == dmcmd_no_wait("/bin/sh", 4, ICWMP_SCRIPT, "apply",
			       "download", VCF_FILE_TYPE))
		blobmsg_add_u32(bb, "status", 1);
	else
		blobmsg_add_u32(bb, "status", 0);

	return SUCCESS;
}

static struct op_cmd operate_helper[] = {
	{"Device.Reboot", reboot_device},
	{"Device.FactoryReset", factory_reset},
	{"Device.IP.Interface.*.Reset", network_interface_reset},
	{"Device.PPP.Interface.*.Reset", network_interface_reset},
	{"Device.WiFi.Reset", wireless_reset},
	{"Device.WiFi.AccessPoint.*.Security.Reset", ap_security_reset},
	{"Device.DHCPv4.Client.*.Renew", dhcp_client_renew},
	{"Device.DHCPv6.Client.*.Renew", dhcp_client_renew},
	{"Device.DeviceInfo.VendorConfigFile.*.Backup", vendor_conf_backup},
	{"Device.DeviceInfo.VendorConfigFile.*.Restore", vendor_conf_restore},
	{"Device.WiFi.NeighboringWiFiDiagnostic", fetch_neighboring_wifi_diagnostic}
	//{"Device.DeviceInfo.VendorConfigFile.*.Restore", blob_parser},
	//{"Device.DeviceInfo.VendorLogFile.*.Upload", blob_parser},
	//{"Device.IP.Diagnostics.IPPing", blob_parser},
	//{"Device.IP.Diagnostics.TraceRoute", blob_parser},
	//{"Device.IP.Diagnostics.DownloadDiagnostics", blob_parser},
	//{"Device.IP.Diagnostics.UploadDiagnostics", blob_parser},
	//{"Device.IP.Diagnostics.UDPEchoDiagnostics", blob_parser},
	//{"Device.IP.Diagnostics.ServerSelectionDiagnostics", blob_parser},
	//{"Device.DNS.Diagnostics.NSLookupDiagnostics", blob_parser}
};

static opr_ret_t operate_on_node(struct blob_buf *bb, char *path, struct
				 blob_attr *bv)
{
	uint8_t len=0;
	DEBUG("entry |%s| |%s|", path, bv->data);

	len = ARRAY_SIZE(operate_helper);
	for(uint8_t i=0; i<len; i++) {
		if(match(path, operate_helper[i].name)) {
			return(operate_helper[i].opt(bb, path, bv));
		}
	}
	return CMD_NOT_FOUND;
}

opr_ret_t create_operate_response(struct blob_buf *bb, char *cmd, struct
				  blob_attr *bv)
{
	pathnode *p=head;

	DEBUG("entry");

	while(p!=NULL) {
		char tmp[NAME_MAX] = {'\0'};
		sprintf(tmp, "%s%s", p->ref_path, cmd);
		switch(operate_on_node(bb, tmp, bv)) {
			case CMD_NOT_FOUND:
				blobmsg_add_u32(bb, "status", 0);
				blobmsg_add_string(bb, "error", "method not supported");
				break;
			case UBUS_INVALID_ARGUMENTS:
				return(UBUS_INVALID_ARGUMENTS);
			case SUCCESS:
			case FAIL:
				break;
			default:
				ERR("Case not defined");
		}
		p=p->next;
	}
	deleteList();
	return SUCCESS;
}
