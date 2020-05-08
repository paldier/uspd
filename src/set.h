#ifndef SET_H
#define SET_H

void create_set_response(struct blob_buf *bb, char *value);
void set_multiple_values(struct blob_buf *bb, struct blob_attr *blob_value);
#endif /* SET_H */

