#ifndef GET_H
#define GET_H

void create_response(struct blob_buf *bb, char *qpath);
void create_raw_response(struct blob_buf *bb);
void create_name_response(struct blob_buf *bb);
void create_inst_name_response(struct blob_buf *bb);

#endif /* GET_H */