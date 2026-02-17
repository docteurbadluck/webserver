#include "test.h"

void test_http_1_0(void)
{
    TEST_MESSAGE("GET /website_name/ (HTTP 1.0)");

    http_response_t res =
        make_request_and_parse_http_version_1_0("GET /website_name/", 0);

    TEST_ASSERT(strncmp(res.headers, "HTTP/1.0 200", 12) == 0);
    assert_header_contains(res.headers, "Connection: close");
}

void test_http_1_1(void)
{
    TEST_MESSAGE("GET /website_name/ (HTTP 1.1)");

    http_response_t res =
        make_request_and_parse("GET /website_name/");

    TEST_ASSERT(strncmp(res.headers, "HTTP/1.1 200", 12) == 0);

    assert_header_not_contains(res.headers, "Connection: close");
}


