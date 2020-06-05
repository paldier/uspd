#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <libubus.h>
#include <libubox/blobmsg_json.h>
#include <libubox/blobmsg.h>

#include <json-c/json.h>

#include "test_utils.h"

int usp_get_handler(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int usp_add_del_handler(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int usp_set(struct ubus_context *ctx, struct ubus_object *obj, \
                struct ubus_request_data *req, const char *method, \
                struct blob_attr *msg);

int usp_operate(struct ubus_context *ctx, struct ubus_object *obj, \
                struct ubus_request_data *req, const char *method, \
                struct blob_attr *msg);

struct test_ctx {
	struct blob_buf bb;
	struct ubus_object usp;
	struct ubus_object usp_raw;
	FILE *fp;
};


static int group_setup(void **state)
{
	struct test_ctx *ctx = calloc(1, sizeof(struct test_ctx));
	if (!ctx)
		return -1;

	remove("/tmp/test.log");
	ctx->usp.name = "usp";
	ctx->usp_raw.name = "usp.raw";
	memset(&ctx->bb, 0, sizeof(struct blob_buf));
	*state = ctx;
	return 0;
}

static int setup(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	remove("/tmp/test.log");
	blob_buf_init(&ctx->bb, 0);
	return 0;
}

static int group_teardown(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;

	blob_buf_free(&ctx->bb);
	free(ctx);
	remove("/tmp/test.log");
	return 0;
}

static void test_api_usp_get_Device_DeviceInfo_Manufacturer(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->usp;
	struct json_object *jobj, *tmp;

	blobmsg_add_string(bb, "path", GET_PATH);
	blobmsg_add_string(bb, "proto", GET_PROTO);

	usp_get_handler(NULL, obj, NULL, "get", bb->head);
	jobj = json_object_from_file("/tmp/test.log");
	assert_non_null(jobj);

	json_object_object_get_ex(jobj, "Manufacturer", &tmp);
	assert_string_equal(json_object_get_string(tmp), "iopsys");

	json_object_put(jobj);
	return;
}

static void test_api_usp_add_object_Device_Users_User(void **state)
{
        struct test_ctx *ctx = (struct test_ctx *) *state;
        struct blob_buf *bb = &ctx->bb;
        struct ubus_object *obj = &ctx->usp;
        struct json_object *jobj, *tmp;

        blobmsg_add_string(bb, "path", "Device.Users.User.");
        blobmsg_add_string(bb, "proto", GET_PROTO);

        usp_add_del_handler(NULL, obj, NULL, "add_object", bb->head);
	jobj = json_object_from_file("/tmp/test.log");
        assert_non_null(jobj);

	json_object_object_get_ex(jobj, "status", &tmp);
	assert_string_equal(json_object_get_string(tmp), "true");

        json_object_put(jobj);
        return;
}

static void test_api_usp_del_Device_Users_User(void **state)
{
        struct test_ctx *ctx = (struct test_ctx *) *state;
        struct blob_buf *bb = &ctx->bb;
        struct ubus_object *obj = &ctx->usp;
        struct json_object *jobj, *tmp, *array_obj, *array_index_obj;

        blobmsg_add_string(bb, "path", "Device.Users.User.[Username==\"user2\"]");
        blobmsg_add_string(bb, "proto", GET_PROTO);

        usp_add_del_handler(NULL, obj, NULL, "del_object", bb->head);
	jobj = json_object_from_file("/tmp/test.log");
        assert_non_null(jobj);

	array_obj = json_object_object_get(jobj, "parameters");
        array_index_obj = json_object_array_get_idx(array_obj, 0);

        tmp = json_object_object_get(array_index_obj, "parameter");
        assert_string_equal(json_object_get_string(tmp), "Device.Users.User.2.");

        tmp = json_object_object_get(array_index_obj, "status");
        assert_string_equal(json_object_get_string(tmp), "true");

        json_object_put(jobj);
        return;
}

static void test_api_usp_instances_Device_Users_User(void **state)
{
        struct test_ctx *ctx = (struct test_ctx *) *state;
        struct blob_buf *bb = &ctx->bb;
        struct ubus_object *obj = &ctx->usp;
	struct json_object *jobj, *tmp, *array_obj, *array_index_obj;

        blobmsg_add_string(bb, "path", "Device.Users.User.");
        blobmsg_add_string(bb, "proto", GET_PROTO);

        usp_get_handler(NULL, obj, NULL, "instances", bb->head);
	jobj = json_object_from_file("/tmp/test.log");

        assert_non_null(jobj);

	array_obj = json_object_object_get(jobj, "parameters");
        array_index_obj = json_object_array_get_idx(array_obj, 0);

        tmp = json_object_object_get(array_index_obj, "parameter");

        assert_string_equal(json_object_get_string(tmp), "Device.Users.User.1");

        json_object_put(jobj);
        return;
}

static void test_api_usp_set_Device_Users_User(void **state)
{
        struct test_ctx *ctx = (struct test_ctx *) *state;
        struct blob_buf *bb = &ctx->bb;
        struct ubus_object *obj = &ctx->usp;
        struct json_object *jobj, *tmp, *array_obj, *array_index_obj;

        blobmsg_add_string(bb, "path", "Device.Users.User.2.Username");
        blobmsg_add_string(bb, "value", "user2");

        usp_set(NULL, obj, NULL, "set", bb->head);
	jobj = json_object_from_file("/tmp/test.log");
        assert_non_null(jobj);

	array_obj = json_object_object_get(jobj, "parameters");
        array_index_obj = json_object_array_get_idx(array_obj, 0);

        tmp = json_object_object_get(array_index_obj, "status");
        assert_string_equal(json_object_get_string(tmp), "true");

	tmp = json_object_object_get(array_index_obj, "path");
        assert_string_equal(json_object_get_string(tmp), "Device.Users.User.2.Username");

        json_object_put(jobj);
        return;
}

static void test_api_usp_operate_reset(void **state)
{
        struct test_ctx *ctx = (struct test_ctx *) *state;
        struct blob_buf *bb = &ctx->bb;
        struct ubus_object *obj = &ctx->usp;
        struct json_object *jobj, *tmp;

        blobmsg_add_string(bb, "path", "Device.IP.Interface.[Name==\"wan\"]");
        blobmsg_add_string(bb, "action", "Reset");

        usp_operate(NULL, obj, NULL, "operate", bb->head);
	jobj = json_object_from_file("/tmp/test.log");
        assert_non_null(jobj);

	json_object_object_get_ex(jobj, "status", &tmp);
        assert_int_equal(json_object_get_boolean(tmp), 0);

        json_object_put(jobj);
        return;
}


static void test_api_usp_object_name_Device_Users_User(void **state)
{
        struct test_ctx *ctx = (struct test_ctx *) *state;
        struct blob_buf *bb = &ctx->bb;
        struct ubus_object *obj = &ctx->usp;
	struct json_object *jobj, *tmp, *array_obj, *array_index_obj;

        blobmsg_add_string(bb, "path", "Device.Users.User.");
        blobmsg_add_string(bb, "proto", GET_PROTO);

        usp_get_handler(NULL, obj, NULL, "object_names", bb->head);
	jobj = json_object_from_file("/tmp/test.log");
        assert_non_null(jobj);

	array_obj = json_object_object_get(jobj, "parameters");
        array_index_obj = json_object_array_get_idx(array_obj, 0);

        tmp = json_object_object_get(array_index_obj, "parameter");
        assert_string_equal(json_object_get_string(tmp), "Device.Users.User.1.Alias");

        json_object_put(jobj);
        return;
}

static void test_api_usp_raw_get_Device_DeviceInfo_Manufacturer(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
        struct blob_buf *bb = &ctx->bb;
        struct ubus_object *obj = &ctx->usp_raw;
        struct json_object *jobj, *tmp, *array_obj, *array_index_obj;

        blobmsg_add_string(bb, "path", GET_PATH);
        blobmsg_add_string(bb, "proto", GET_PROTO);

        usp_get_handler(NULL, obj, NULL, "get", bb->head);
        jobj = json_object_from_file("/tmp/test.log");
        assert_non_null(jobj);

	array_obj = json_object_object_get(jobj, "parameters");
	array_index_obj = json_object_array_get_idx(array_obj, 0);

	tmp = json_object_object_get(array_index_obj, "parameter");

        assert_string_equal(json_object_get_string(tmp), "Device.DeviceInfo.Manufacturer");

	tmp = json_object_object_get(array_index_obj, "value");
	assert_string_equal(json_object_get_string(tmp), "iopsys");

        json_object_put(jobj);
        return;
}


static void test_api_usp_raw_add_object_Device_Users_User(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
        struct blob_buf *bb = &ctx->bb;
        struct ubus_object *obj = &ctx->usp_raw;
        struct json_object *jobj, *tmp;

        blobmsg_add_string(bb, "path", "Device.Users.User.");
        blobmsg_add_string(bb, "proto", GET_PROTO);

        usp_add_del_handler(NULL, obj, NULL, "add_object", bb->head);
        jobj = json_object_from_file("/tmp/test.log");
        assert_non_null(jobj);

        json_object_object_get_ex(jobj, "status", &tmp);
        assert_string_equal(json_object_get_string(tmp), "true");

        json_object_put(jobj);
        return;
}


static void test_api_usp_raw_del_Device_Users_User(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
        struct blob_buf *bb = &ctx->bb;
        struct ubus_object *obj = &ctx->usp_raw;
        struct json_object *jobj, *tmp, *array_obj, *array_index_obj;

        blobmsg_add_string(bb, "path", "Device.Users.User.[Username==\"user2\"]");
        blobmsg_add_string(bb, "proto", GET_PROTO);

        usp_add_del_handler(NULL, obj, NULL, "del_object", bb->head);
        jobj = json_object_from_file("/tmp/test.log");
        assert_non_null(jobj);

	array_obj = json_object_object_get(jobj, "parameters");
        array_index_obj = json_object_array_get_idx(array_obj, 0);

        tmp = json_object_object_get(array_index_obj, "parameter");
        assert_string_equal(json_object_get_string(tmp), "Device.Users.User.2.");

        tmp = json_object_object_get(array_index_obj, "status");
        assert_string_equal(json_object_get_string(tmp), "true");

        json_object_put(jobj);
        return;
}

static void test_api_usp_raw_instances_Device_Users_User(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
        struct blob_buf *bb = &ctx->bb;
        struct ubus_object *obj = &ctx->usp_raw;
        struct json_object *jobj, *tmp, *array_obj, *array_index_obj;

        blobmsg_add_string(bb, "path", "Device.Users.User.");
        blobmsg_add_string(bb, "proto", GET_PROTO);

        usp_get_handler(NULL, obj, NULL, "instances", bb->head);
        jobj = json_object_from_file("/tmp/test.log");
        assert_non_null(jobj);

        array_obj = json_object_object_get(jobj, "parameters");
        array_index_obj = json_object_array_get_idx(array_obj, 0);

        tmp = json_object_object_get(array_index_obj, "parameter");

        assert_string_equal(json_object_get_string(tmp), "Device.Users.User.1");

        json_object_put(jobj);
        return;
}


static void test_api_usp_raw_set_Device_Users_User(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
        struct blob_buf *bb = &ctx->bb;
        struct ubus_object *obj = &ctx->usp_raw;
        struct json_object *jobj, *tmp, *array_obj, *array_index_obj;

        blobmsg_add_string(bb, "path", "Device.Users.User.2.Username");
        blobmsg_add_string(bb, "value", "user2");

        usp_set(NULL, obj, NULL, "set", bb->head);
        jobj = json_object_from_file("/tmp/test.log");
        assert_non_null(jobj);

	array_obj = json_object_object_get(jobj, "parameters");
        array_index_obj = json_object_array_get_idx(array_obj, 0);

        tmp = json_object_object_get(array_index_obj, "status");
        assert_string_equal(json_object_get_string(tmp), "true");

	tmp = json_object_object_get(array_index_obj, "path");
        assert_string_equal(json_object_get_string(tmp), "Device.Users.User.2.Username");

        json_object_put(jobj);
        return;
}

static void test_api_usp_raw_operate_reset(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
        struct blob_buf *bb = &ctx->bb;
        struct ubus_object *obj = &ctx->usp_raw;
        struct json_object *jobj, *tmp;

        blobmsg_add_string(bb, "path", "Device.IP.Interface.[Name==\"wan\"]");
        blobmsg_add_string(bb, "action", "Reset");

        usp_operate(NULL, obj, NULL, "operate", bb->head);
        jobj = json_object_from_file("/tmp/test.log");
        assert_non_null(jobj);

        json_object_object_get_ex(jobj, "status", &tmp);
        assert_int_equal(json_object_get_boolean(tmp), 0);

        json_object_put(jobj);
        return;
}

static void test_api_usp_raw_object_name_Device_Users_User(void **state)
{
        struct test_ctx *ctx = (struct test_ctx *) *state;
        struct blob_buf *bb = &ctx->bb;
        struct ubus_object *obj = &ctx->usp_raw;
	struct json_object *jobj, *tmp, *array_obj, *array_index_obj;

        blobmsg_add_string(bb, "path", "Device.Users.User.");
        blobmsg_add_string(bb, "proto", GET_PROTO);

        usp_get_handler(NULL, obj, NULL, "object_names", bb->head);
        jobj = json_object_from_file("/tmp/test.log");
        assert_non_null(jobj);

        array_obj = json_object_object_get(jobj, "parameters");
        array_index_obj = json_object_array_get_idx(array_obj, 0);

        tmp = json_object_object_get(array_index_obj, "parameter");
        assert_string_equal(json_object_get_string(tmp), "Device.Users.User.1.Alias");

        json_object_put(jobj);
        return;
}

static void test_api_usp_get_cwmp_Device_DeviceInfo_Manufacturer(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
        struct blob_buf *bb = &ctx->bb;
        struct ubus_object *obj = &ctx->usp;
        struct json_object *jobj, *tmp;

        blobmsg_add_string(bb, "path", GET_PATH);
        blobmsg_add_string(bb, "proto", GET_CWMP_PROTO);

        usp_get_handler(NULL, obj, NULL, "get", bb->head);
        jobj = json_object_from_file("/tmp/test.log");
        assert_non_null(jobj);

        json_object_object_get_ex(jobj, "Manufacturer", &tmp);
        assert_string_equal(json_object_get_string(tmp), "iopsys");

        json_object_put(jobj);
        return;
}

int ubus_send_reply(struct ubus_context *ctx, struct ubus_request_data *req,
                    struct blob_attr *msg)
{
	char *str;
	FILE *fp;

	fp = fopen("/tmp/test.log", "w");
	if (!fp) {
		printf("failed to open file\n");
		return -1;
	}

	if (!msg)
		return -1;

	str = blobmsg_format_json_indent(msg, true, -1);
	fprintf(fp, "%s", str);

	fclose(fp);
	free(str);
        return 0;
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_api_usp_get_Device_DeviceInfo_Manufacturer, setup),
		cmocka_unit_test_setup(test_api_usp_instances_Device_Users_User, setup),
		cmocka_unit_test_setup(test_api_usp_add_object_Device_Users_User, setup),
		cmocka_unit_test_setup(test_api_usp_set_Device_Users_User, setup),
		cmocka_unit_test_setup(test_api_usp_del_Device_Users_User, setup),
		cmocka_unit_test_setup(test_api_usp_operate_reset, setup),
		cmocka_unit_test_setup(test_api_usp_object_name_Device_Users_User, setup),
		cmocka_unit_test_setup(test_api_usp_raw_get_Device_DeviceInfo_Manufacturer, setup),
                cmocka_unit_test_setup(test_api_usp_raw_instances_Device_Users_User, setup),
		cmocka_unit_test_setup(test_api_usp_raw_add_object_Device_Users_User, setup),
		cmocka_unit_test_setup(test_api_usp_raw_set_Device_Users_User, setup),
                cmocka_unit_test_setup(test_api_usp_raw_del_Device_Users_User, setup),
                cmocka_unit_test_setup(test_api_usp_raw_operate_reset, setup),
                cmocka_unit_test_setup(test_api_usp_raw_object_name_Device_Users_User, setup),
                cmocka_unit_test_setup(test_api_usp_get_cwmp_Device_DeviceInfo_Manufacturer, setup)
	};

	return cmocka_run_group_tests(tests, group_setup, group_teardown);
}

