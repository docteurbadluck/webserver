#include "test.h"

void setUp(void) {}
void tearDown(void) {}

void test_get()
{
	RUN_TEST(test_get_200);
	RUN_TEST(test_get_200_big_size);
	RUN_TEST(test_get_403);
	RUN_TEST(test_get_404);
	RUN_TEST(test_get_405);
	RUN_TEST(test_get_500);
	RUN_TEST(test_get_binary);
}

void test_post()
{
	RUN_TEST(test_post_200);
	//RUN_TEST(test_post_403);
	RUN_TEST(test_post_404);
	RUN_TEST(test_post_405);
	RUN_TEST(test_post_413);
	RUN_TEST(test_post_500);
}

void test_delete()
{
	RUN_TEST(test_delete_200);
	RUN_TEST(test_delete_404);
	//RUN_TEST(test_delete_403);
	RUN_TEST(test_delete_405);
}

void test_redirect()
{
	RUN_TEST(test_redirect_301);
	RUN_TEST(test_redirect_302);
	RUN_TEST(test_redirect_wrong_name);
}

void test_connection()
{
	RUN_TEST(test_http_1_0);
	RUN_TEST(test_http_1_1);
}


void test_multiple_request()
{
	RUN_TEST(test_two_get_requests_in_one_tcp_packet);
    RUN_TEST(test_http10_close_after_first_request);
    RUN_TEST(test_five_get_requests_in_one_tcp_packet);
}
	
void test_CGI(void)
{
    TEST_MESSAGE("GET /website_name/CGI/test.py");
	system("rm -f CGI.rec");
    system("curl -s http://127.0.0.1:8080/website_name/CGI/test.py -o CGI.rec");

    FILE *f = fopen("CGI.rec", "r");
    TEST_ASSERT_NOT_NULL(f);

    char buffer[1024];
    size_t read = fread(buffer, 1, sizeof(buffer) - 1, f);
    buffer[read] = '\0';
    fclose(f);

    TEST_ASSERT_NOT_NULL(strstr(buffer, "<html><body><h1>CGI called</h1></body></html>"));
}

int main(void)
{

	UNITY_BEGIN();

	test_get();
	test_post();
	test_delete();
	test_redirect();
	test_connection();	
	test_multiple_request();
	RUN_TEST(test_CGI);
	return UNITY_END();
}
