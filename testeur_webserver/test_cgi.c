#include "test.h"

/* Helper: send raw request, read until connection closes */
static int cgi_request(const char *request, char *response, size_t max_len)
{
    int sock;
    struct sockaddr_in server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        close(sock);
        return -1;
    }

    send(sock, request, strlen(request), 0);

    size_t total = 0;
    int r;
    while ((r = recv(sock, response + total, max_len - total - 1, 0)) > 0)
        total += r;
    response[total] = '\0';
    close(sock);
    return total;
}

void test_cgi_python(void)
{
    TEST_MESSAGE("GET /website_name/CGI/hello.py should return Python CGI page");

    char response[16384];
    const char *req =
        "GET /website_name/CGI/hello.py HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "\r\n";

    int bytes = cgi_request(req, response, sizeof(response));
    TEST_ASSERT_TRUE_MESSAGE(bytes > 0, "Server did not respond");
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(response, "200 OK"), "Expected 200 OK");
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(response, "CGI Greeter"), "Expected CGI Greeter page");
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(response, "Enter your name"), "Expected name prompt");
}

void test_cgi_post(void)
{
    TEST_MESSAGE("POST /website_name/CGI/hello.py with name should greet user");

    char response[16384];
    const char *body = "name=Jakob";
    char req[2048];
    snprintf(req, sizeof(req),
        "POST /website_name/CGI/hello.py HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s",
        (int)strlen(body), body);

    int bytes = cgi_request(req, response, sizeof(response));
    TEST_ASSERT_TRUE_MESSAGE(bytes > 0, "Server did not respond");
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(response, "200 OK"), "Expected 200 OK");
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(response, "Hello, Jakob!"), "Expected personalized greeting");
}

void test_cgi_not_found(void)
{
    TEST_MESSAGE("GET /website_name/CGI/nonexistent.py should return 404");

    char response[8192];
    const char *req =
        "GET /website_name/CGI/nonexistent.py HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "\r\n";

    int bytes = cgi_request(req, response, sizeof(response));
    TEST_ASSERT_TRUE_MESSAGE(bytes > 0, "Server did not respond");
    TEST_ASSERT_NOT_NULL_MESSAGE(strstr(response, "404"), "Expected 404 for nonexistent CGI");
}
