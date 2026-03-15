#include "test.h"

void test_post_200(void)
{
    //TEST_MESSAGE("POST /website_name/upload/valid.txt should return 200");
    http_response_t res = post_request("/website_name/upload/test_200.txt", "hello world");
    TEST_ASSERT_EQUAL_INT(200, res.status_code);
}

void test_post_403(void)
{
    //TEST_MESSAGE("POST /website_name/upload/../test/file.txt should return 403");
    http_response_t res = post_request("/website_name/upload/restricted/test_403.txt", "text");
    TEST_ASSERT_EQUAL_INT(403, res.status_code);
}

void test_post_404(void)
{
    //TEST_MESSAGE("POST /website_name/upload/in/nonexistent_dir.txt should return 404");
    http_response_t res = post_request("/website_name/upload/in/nonexistent_dir.txt", "text");
    TEST_ASSERT_EQUAL_INT(404, res.status_code);
}


void test_post_405(void)
{
    //TEST_MESSAGE("POST /website_name/only_get_directory/file.txt should return 405");
    http_response_t res = post_request("/website_name/readonly/file.txt", "data");
    TEST_ASSERT_EQUAL_INT(405, res.status_code);
}

void test_post_413(void)
{
    //TEST_MESSAGE("POST /website_name/upload/bigfile.txt should return 413");

    int body_size = 2000100;
    char *big_body = (char *)malloc(body_size);

	int i = 0;

	while (i < body_size - 1)
	{
		big_body[i] = 'a';
		i++;
	}
	big_body[body_size - 1] = 0;

    http_response_t res = post_request("/website_name/upload/bigfile.txt", big_body);
    free(big_body);
    TEST_ASSERT_EQUAL_INT(413, res.status_code);
}

void test_post_500(void)
{
    //TEST_MESSAGE("POST /cause_internal_error should return 500");
    
    http_response_t res = post_request("/cause_internal_error", "data");
    TEST_ASSERT_EQUAL_INT(500, res.status_code);
}

