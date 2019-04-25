#ifndef SET_H
#define SET_H
enum {
	DM_SET_PATH,
	DM_SET_VALUE,
	DM_SET_VALUE_TABLE,
	__DMSET_MAX,
};

void create_set_response(struct blob_buf *bb, char *value);
void set_multiple_values(struct blob_buf *bb, struct blob_attr *blob_value);
#endif /* SET_H */

