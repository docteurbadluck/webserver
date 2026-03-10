#include "test.h"

void test_redirect_301(void)
{
    //TEST_MESSAGE("GET /old-page/file.txt should return 301");
    http_response_t res =
        make_request_and_parse("GET /old-page/file.txt");
    TEST_ASSERT_EQUAL_INT(301, res.status_code);
}

void test_redirect_302(void)
{
    //TEST_MESSAGE("GET /old_tmp/file.txt should return 302");
    http_response_t res =
        make_request_and_parse("GET /old_tmp/file.txt");

    TEST_ASSERT_EQUAL_INT(302, res.status_code);
}


void test_redirect_wrong_name(void)
{
    //TEST_MESSAGE("GET /wrong/file.txt should return 404");
    http_response_t res =
        make_request_and_parse("GET /wrong/file.txt");

	(void)res;
    TEST_ASSERT_EQUAL_INT(404, res.status_code);
}


