/*
 * usp.h: Header file for usp.c
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
 
#ifndef USP_H
#define USP_H
#include <syslog.h>
#include <libdatamodel/dmentry.h>
#include <stdbool.h>

extern struct ubus_object usp_object;

inline void cwmp_init(struct dmctx *dm_ctx) {
	dm_ctx_init(dm_ctx, DM_CWMP, AMD_2, INSTANCE_MODE_NUMBER);
}

inline void cwmp_cleanup(struct dmctx *dm_ctx) {
	dm_ctx_clean(dm_ctx);
}

inline bool is_str_eq(char *s1, char *s2) {
	if(0==strcmp(s1, s2))
		return true;

	return false;
}
typedef struct pathnode {
	char *ref_path;
	struct pathnode *next;
} pathnode;

#define DEBUG_ENABLED 0
#define DEBUG(fmt, args...) \
do { \
	if (DEBUG_ENABLED) \
		syslog(LOG_DEBUG, "[%s:%d] " fmt, __func__, __LINE__, ##args); \
} while (0)

#define INFO(fmt, args...) \
do { \
	syslog(LOG_INFO, "[%s:%d] " fmt, __func__, __LINE__, ##args); \
} while (0)

#define ERR(fmt, args...) \
do { \
	syslog(LOG_ERR, "[%s:%d] " fmt, __func__, __LINE__, ##args); \
} while (0)

#endif /* USP_H */
