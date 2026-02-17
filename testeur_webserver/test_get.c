#include "test.h"


void test_get_200(void)
{
	TEST_MESSAGE("GET website_name/ should return 200");

	http_response_t res = make_request_and_parse("GET /website_name/");
	TEST_ASSERT_EQUAL_INT(200, res.status_code);

	TEST_MESSAGE("GET / (root) should return 200");

	res = make_request_and_parse("GET /");  
	TEST_ASSERT_EQUAL_INT(200, res.status_code);

	TEST_MESSAGE("GET website_name/ should return index.html");
	res = make_request_and_parse("GET /website_name/");
	TEST_ASSERT(strstr(res.body, "Hello") != NULL);

	TEST_MESSAGE("GET website_name/test_without_index should return list of directory");
	res = make_request_and_parse("GET /website_name/test_without_index/");
	TEST_ASSERT_EQUAL_INT(200, res.status_code);
	TEST_ASSERT(strstr(res.body, "Index of") != NULL);
}

// par la suite j'aimerai tester de recevoir 500 mega avec des chunked
//fonctionne si la taille alouer au buffer est assez grand 
void test_get_200_big_size(void)
{
	TEST_MESSAGE("GET website_name/test/big_size_file.txt should return 200 and be completely send");
	http_response_t res = make_request_and_parse("GET /website_name/test/big_size_file.txt");
	TEST_ASSERT_EQUAL_INT(11000, strlen(res.body));
}


void test_get_200_extra_big_size(void)
{
	TEST_MESSAGE("GET website_name/test/big_size_file.txt should return 200 and be completely send");
	http_response_stream_t res = parse_http_response_stream("GET /website_name/test/extra_big_size_file.txt");
	TEST_ASSERT_EQUAL_INT(1100000, strlen(res.body));
}



void test_get_403(void)
{
    TEST_MESSAGE("GET /website_name/forbiden_directory/ should return 403");
    http_response_t res = make_request_and_parse("GET /website_name/forbiden_directory/");
    TEST_ASSERT_EQUAL_INT(403, res.status_code);
}

void test_get_404(void)
{
	TEST_MESSAGE("GET website_name/not_existing_file should return 404");

	http_response_t res = make_request_and_parse("GET /website_name/not_existing");
	TEST_ASSERT_EQUAL_INT(404, res.status_code);

    TEST_MESSAGE("GET /website_name/only_post_directory/not_existing_file should return 404");

	res = make_request_and_parse("GET /website_name/only_post_directory/not_existing_file");
    TEST_ASSERT_EQUAL_INT(404, res.status_code);
}


void test_get_405(void)
{
    TEST_MESSAGE("GET /website_name/only_post_directory/ should return 405");
    http_response_t res = make_request_and_parse("GET /website_name/only_post_directory/");
    TEST_ASSERT_EQUAL_INT(405, res.status_code);

    TEST_MESSAGE("GET /website_name/only_get_directory/ should return 200");
    res = make_request_and_parse("GET /website_name/only_get_directory/");
    TEST_ASSERT_EQUAL_INT(200, res.status_code);

}

void test_get_500(void)
{
    TEST_MESSAGE("GET /cause_internal_error should return 500");
    http_response_t res = make_request_and_parse("GET /cause_internal_error");
    TEST_ASSERT_EQUAL_INT(500, res.status_code);
}

void test_get_binary(void)
{
    TEST_MESSAGE("GET /website_name/test/test.bin (HTTP 1.1)");
	system("rm -f received.bin");
	system("curl -s http://127.0.0.1:8080/website_name/test/test.bin -o received.bin");
	int res = system("cmp test.bin received.bin");	
	
    TEST_ASSERT_EQUAL_INT(0, res);
}
