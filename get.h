#ifndef GET_H
#define GET_H
enum {
	DMPATH_NAME,
	__DM_MAX,
};

void create_response(struct blob_buf *bb, char *qpath);
#endif /* GET_H */
