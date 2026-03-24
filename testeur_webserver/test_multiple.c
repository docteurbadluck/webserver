#include "test.h"

void test_two_get_requests_in_one_tcp_packet(void)
{
    //TEST_MESSAGE("HTTP pipelining: two GET in one send");

    system(
        "printf \"GET /a HTTP/1.1\r\n\r\n"
        "GET /b HTTP/1.1\r\n\r\n\" | "
        "nc -q 2 127.0.0.1 8080 > response.txt"
    );

    FILE *f = fopen("response.txt", "r");
    TEST_ASSERT_NOT_NULL(f);

    char buffer[65536];
    size_t total = 0, r;
    while ((r = fread(buffer + total, 1, sizeof(buffer) - total - 1, f)) > 0)
        total += r;
    buffer[total] = '\0';

    fclose(f);

    int count = 0;
    char *p = buffer;
    while ((p = strstr(p, "HTTP/")) != NULL)
    {
        count++;
        p += 5;
    }

    TEST_ASSERT_EQUAL_INT_MESSAGE(
        2, count,
        "Server did not answer twice to pipelined requests"
    );
}

void test_http10_close_after_first_request(void)
{
    //TEST_MESSAGE("HTTP/1.0 close: server should close after first response");

    system(
        "printf \"GET /first HTTP/1.0\r\nConnection: close\r\n\r\n"
        "GET /second HTTP/1.0\r\n\r\n\" | "
        "nc -q 0 127.0.0.1 8080 > response.txt"
    );

    FILE *f = fopen("response.txt", "r");
    TEST_ASSERT_NOT_NULL(f);

    char buffer[8192];
    size_t read = fread(buffer, 1, sizeof(buffer) - 1, f);
    buffer[read] = '\0';
    fclose(f);
    int count = 0;
    char *p = buffer;
    while ((p = strstr(p, "HTTP/")) != NULL)
    {
        count++;
        p += 5;
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        1, count,
        "Server should close after first HTTP/1.0 response"
    );
}

void test_five_get_requests_in_one_tcp_packet(void)
{
    //TEST_MESSAGE("HTTP pipelining: five GET in one send");

    system(
        "printf \"GET /a HTTP/1.1\r\n\r\n"
        "GET /a HTTP/1.1\r\n\r\n"
        "GET /a HTTP/1.1\r\n\r\n"
        "GET /a HTTP/1.1\r\n\r\n"
        "GET /b HTTP/1.1\r\n\r\n\" | "
        "nc -q 1 127.0.0.1 8080 > response.txt"
    );

    FILE *f = fopen("response.txt", "r");
    TEST_ASSERT_NOT_NULL(f);
    char buffer[65536];
    size_t total = 0, r;
    while ((r = fread(buffer + total, 1,
                      sizeof(buffer) - total - 1, f)) > 0)
    {
        total += r;
    }
    buffer[total] = '\0';
    fclose(f);
    int count = 0;
    char *p = buffer;
    while ((p = strstr(p, "HTTP/")) != NULL)
    {
        count++;
        p += 5;
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        5, count,
        "Server did not answer five times to pipelined requests"
    );
}


