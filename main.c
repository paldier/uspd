/*
 * main.c: USP deamon
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
 
#include <stdio.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <libubox/blobmsg.h>
#include <libubox/uloop.h>
#include <libubus.h>

#include "usp.h"

static void usp_init(struct ubus_context *ctx)
{
	int ret;

	ret = ubus_add_object(ctx, &usp_object);
	if (ret)
		ERR("Failed to add 'usp' ubus object: %s\n", ubus_strerror(ret));

	uloop_run();
}

int main()
{
	const char *ubus_socket = NULL;
	struct ubus_context *ctx = NULL;

	openlog("uspd", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	uloop_init();
	ctx = ubus_connect(ubus_socket);
	if (!ctx) {
		fprintf(stderr, "Failed to connect to ubus\n");
		return -1;
	}
	ubus_add_uloop(ctx);
	usp_init(ctx);

	uloop_run();
	ubus_free(ctx);
	uloop_done();
	closelog();

	return 0;
}
