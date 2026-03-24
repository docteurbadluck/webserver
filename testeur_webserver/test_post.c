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

    int body_size = 104857700; /* just over 100MB config limit */
    int sock;
    struct sockaddr_in server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    TEST_ASSERT_TRUE_MESSAGE(sock >= 0, "Socket creation failed");

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    int conn = connect(sock, (struct sockaddr *)&server, sizeof(server));
    TEST_ASSERT_TRUE_MESSAGE(conn >= 0, "Connect failed");

    char headers[512];
    snprintf(headers, sizeof(headers),
        "POST /website_name/upload/bigfile.txt HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Length: %d\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n", body_size);
    send(sock, headers, strlen(headers), 0);

    char chunk[4096];
    memset(chunk, 'a', sizeof(chunk));
    int sent_total = 0;
    while (sent_total < body_size)
    {
        int to_send = body_size - sent_total;
        if (to_send > (int)sizeof(chunk))
            to_send = sizeof(chunk);
        int s = send(sock, chunk, to_send, 0);
        if (s <= 0)
            break;
        sent_total += s;
    }

    char response[8192];
    size_t total = 0;
    int r;
    while ((r = recv(sock, response + total, sizeof(response) - total - 1, 0)) > 0)
        total += r;
    response[total] = '\0';
    close(sock);

    http_response_t res = parse_http_response(response);
    TEST_ASSERT_EQUAL_INT(413, res.status_code);
}

void test_post_500(void)
{
    //TEST_MESSAGE("POST /cause_internal_error should return 500");
    
    http_response_t res = post_request("/cause_internal_error", "data");
    TEST_ASSERT_EQUAL_INT(500, res.status_code);
}

