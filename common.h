#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <syslog.h>
#include <regex.h>
#include <sys/param.h>

#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubox/utils.h>
#include <libubus.h>

#include <libdatamodel/dmubus.h>
#include <libdatamodel/dmcommon.h>
#include <libdatamodel/dmentry.h>

#define STRING_DEFAULT 256

typedef struct pathnode {
	char *ref_path;
	struct pathnode *next;
} pathnode;


void cwmp_init(struct dmctx *dm_ctx, char *path);
void cwmp_cleanup(struct dmctx *dm_ctx);
char *cwmp_get_value_by_id(char *id);
bool cwmp_set_value(struct blob_buf *bb, char *path, char *value);
bool cwmp_get_value(char *path, bool fill, char *query_path);
bool cwmp_get_name(char *path);
bool cwmp_get_value_raw(char *path, struct blob_buf *bb);

void filter_results(char *path, int start, int end);

void deleteList();
void printList(bool active);

bool is_str_eq(const char *s1, const char *s2);
bool match(const char *string, const char *pattern);
void prepare_result(struct blob_buf *bb);

bool db_get_value(char *package, char *section, char *option, char **value);

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

#endif /* COMMON_H */
