#ifndef ADD_DEL_H
#define ADD_DEL_H
void add_object(struct blob_buf *bb, char *path, const char *pkey);
void del_object(struct blob_buf *bb, char *path, const char *pkey);
#endif /* ADD_DEL_H */
