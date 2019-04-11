#ifndef OPERATE_H
#define OPERATE_H
enum {
	DM_OPERATE_PATH,
	__DM_OPERATE_MAX,
};

typedef void (*operation) (char *p);

struct op_cmd {
	char *name;
	operation opt;
};

void create_operate_response(struct blob_buf *bb, char *cmd);
#endif /* OPERATE_H */
