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
    TEST_MESSAGE("GET /website_name/CGI/test.py should execute CGI script");

    int sock;
    struct sockaddr_in server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    TEST_ASSERT_TRUE_MESSAGE(sock >= 0, "Socket creation failed");

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    int conn = connect(sock, (struct sockaddr *)&server, sizeof(server));
    TEST_ASSERT_TRUE_MESSAGE(conn >= 0, "Connect failed");

    const char *req = "GET /website_name/CGI/test.py HTTP/1.1\r\nHost: localhost:8080\r\n\r\n";
    send(sock, req, strlen(req), 0);

    /* Read until connection closes (CGI uses Connection: close) */
    char response[8192];
    size_t total = 0;
    int r;
    while ((r = recv(sock, response + total, sizeof(response) - total - 1, 0)) > 0)
        total += r;
    response[total] = '\0';
    close(sock);

    TEST_ASSERT_TRUE_MESSAGE(total > 0, "Server did not respond");
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(response, "200 OK"), "Expected 200 OK status");
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(response, "Hello from CGI"), "Expected CGI output in body");
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
	RUN_TEST(test_cgi_python);
	RUN_TEST(test_cgi_post);
	RUN_TEST(test_cgi_not_found);
	return UNITY_END();
}
