#ifndef SET_H
#define SET_H

void create_set_response(struct blob_buf *bb, char *value, char *key);
void set_multiple_values(struct blob_buf *bb, struct blob_attr *blob_value, char *key);
#endif /* SET_H */

