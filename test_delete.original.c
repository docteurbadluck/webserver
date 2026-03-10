#include "test.h"

void test_delete_200(void)
{
    TEST_MESSAGE("DELETE /website_name/upload/delete_ok.txt should return 200");
    system("echo hello > /home/docteurbadluck/Desktop/rearanged_webserver/www/upload/delete_ok.txt");
    http_response_t res =
        make_request_and_parse("DELETE /website_name/upload/delete_ok.txt");
    TEST_ASSERT_EQUAL_INT(200, res.status_code);
    TEST_ASSERT_EQUAL_INT(-1, access("website_name/upload/delete_ok.txt", F_OK));
}

void test_delete_404(void)
{
    TEST_MESSAGE("DELETE nonexistent file should return 404");
    http_response_t res =
        make_request_and_parse("DELETE /website_name/upload/no_such_file.txt");
    TEST_ASSERT_EQUAL_INT(404, res.status_code);
}

void test_delete_403(void)
{
    TEST_MESSAGE("DELETE in protected folder should return 403");
    http_response_t res =
        make_request_and_parse("DELETE /website_name/protected/readonly.txt");
    TEST_ASSERT_EQUAL_INT(403, res.status_code);
}

void test_delete_405(void)
{
    TEST_MESSAGE("DELETE /website_name/only_get_directory/index.html folder should return 405");
    http_response_t res =
        make_request_and_parse("DELETE /website_name/only_get_directory/index.html");
    TEST_ASSERT_EQUAL_INT(405, res.status_code);
}


