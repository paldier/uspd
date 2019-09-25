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

#include <libbbfdm/dmubus.h>
#include <libbbfdm/dmcommon.h>
#include <libbbfdm/dmentry.h>

#define STRING_DEFAULT 256

typedef struct pathnode {
	char *ref_path;
	struct pathnode *next;
} pathnode;


void bbf_init(struct dmctx *dm_ctx, char *path);
void bbf_cleanup(struct dmctx *dm_ctx);
char *bbf_get_value_by_id(char *id);
bool bbf_set_value(struct blob_buf *bb, char *path, char *value);
bool bbf_get_value(char *path, bool fill, char *query_path);
bool bbf_get_name(char *path);
bool bbf_get_value_raw(char *path, struct blob_buf *bb);
bool get_granular_obj_list(char *path);
int bbf_operate(struct dmctx *dm_ctx, char *path, char *input_params);

void insert(char *data, bool active);
void filter_results(char *path, int start, int end);

void deleteList();
void printList(bool active);

bool is_str_eq(const char *s1, const char *s2);
bool match(const char *string, const char *pattern);
void insert_result(char *name, char *value, char *type);
void delete_result();
void process_result(struct blob_buf *bb, unsigned int len);
void prepare_result(struct blob_buf *bb);

bool db_get_value(char *package, char *section, char *option, char **value);
bool get_uci_option_string(char *package, char *section, char *option, char
			  **value);

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
