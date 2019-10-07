#ifndef ADD_DEL_H
#define ADD_DEL_H
void add_object(struct blob_buf *bb, char *path);
void del_object(struct blob_buf *bb, char *qpath);
#endif /* ADD_DEL_H */
