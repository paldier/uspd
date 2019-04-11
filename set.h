#ifndef SET_H
#define SET_H
enum {
	DM_SET_PATH,
	DM_SET_VALUE,
	__DMSET_MAX,
};

void create_set_response(struct blob_buf *bb, char *value);
#endif /* SET_H */

