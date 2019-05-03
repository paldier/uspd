/*
 * dummy.c: dummy function defination for libdatamodel binding
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

