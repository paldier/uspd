#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <syslog.h>
#include <regex.h>
#include <sys/param.h>
#include <ctype.h>

#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubox/utils.h>
#include <libubus.h>

#include <libbbfdm/dmentry.h>
#include <libbbfdm/dmbbfcommon.h>

#define STRING_DEFAULT 256
#define DELIM '.'

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
void bbf_get_value_blob(char *path, struct blob_buf *bb);
bool bbf_get_inst_name_raw(char *path, struct blob_buf *bb);
bool bbf_get_name_raw(char *path, struct blob_buf *bb);
bool get_granular_obj_list(char *path);
int bbf_operate(struct dmctx *dm_ctx, char *path, char *input_params);

void insert(char *data, bool active);
void filter_results(char *path, size_t start, size_t end);

void deleteList();
void printList(bool active);

bool is_str_eq(const char *s1, const char *s2);
bool match(const char *string, const char *pattern);
void insert_result(char *name, char *value, char *type);
void delete_result();
void process_result(struct blob_buf *bb, unsigned int len);
void prepare_result(struct blob_buf *bb);

bool get_uci_option_string(char *package, char *section, char *option, char **value);

void set_debug_level(unsigned char level);
void print_error(const char *format, ...);
void print_warning(const char *format, ...);
void print_info(const char *format, ...);
void print_debug(const char *format, ...);

#define DEBUG(fmt, args...) \
do { \
	print_debug(fmt, ##args); \
} while (0)

#define INFO(fmt, args...) \
do { \
	print_info(fmt, ##args); \
} while (0)

#define ERR(fmt, args...) \
do { \
	print_error("[%s:%d] " fmt, __func__, __LINE__, ##args); \
} while (0)

#define WARNING(fmt, args...) \
do { \
	print_warning("[%s:%d] " fmt, __func__, __LINE__, ##args); \
} while (0)

#define __unused __attribute__((unused))

#endif /* COMMON_H */
