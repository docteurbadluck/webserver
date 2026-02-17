#include "test.h"

int http_request(const char *request, char *response, size_t max_len)
{
    int sock;
    struct sockaddr_in server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        close(sock);
        return -1;
    }

    send(sock, request, strlen(request), 0);

    size_t total = 0;
    int r;

    // ------------------ Lire jusqu'à la fin des headers ------------------
    char *header_end = NULL;
    while (!header_end)
    {
        r = recv(sock, response + total, max_len - total - 1, 0);
        if (r <= 0) break;
        total += r;
        response[total] = '\0';

        header_end = strstr(response, "\r\n\r\n");
        if (total >= max_len - 1) break;
    }

    if (!header_end) {
        close(sock);
        return -1;
    }

    size_t header_len = header_end - response + 4;

    // ------------------ Lire Content-Length ------------------
    int content_length = 0;
    char *cl = strcasestr(response, "Content-Length:");
    if (cl) {
        content_length = atoi(cl + strlen("Content-Length:"));
    } else {
        close(sock);
        return total;
    }

    // ------------------ Lire le body ENTIER ------------------
    size_t already_read = total - header_len;
    while (already_read < (size_t)content_length)
    {
        r = recv(sock, response + total, max_len - total - 1, 0);
        if (r <= 0) break;

        total += r;
        already_read += r;
    }

    response[total] = '\0';
    close(sock);
    return total;
}



http_response_t parse_http_response(char *raw)
{
    http_response_t res;
    res.status_code = 0;
    res.headers = NULL;
    res.body = NULL;

    if (!raw)
        return res;

    // ------------------ Trouver la ligne de status ------------------
    char *status_line = strstr(raw, "HTTP/");
    if (status_line)
    {
        // Trouver le premier espace après HTTP/x.x
        char *space = strchr(status_line, ' ');
        if (space)
            res.status_code = atoi(space + 1);
    }

    // ------------------ Séparer headers et body ------------------
    char *header_end = strstr(raw, "\r\n\r\n");

    if (header_end)
    {
        size_t headers_len = header_end - raw;

        res.headers = (char *)malloc(headers_len + 1);
        memcpy(res.headers, raw, headers_len);
        res.headers[headers_len] = '\0';

        // Body = après "\r\n\r\n"
        res.body = strdup(header_end + 4);
    }
    else
    {
        // Pas de body trouvé
        res.headers = strdup(raw);
        res.body = strdup("");
    }

    return res;
}

http_response_t make_request_and_parse(const char *request_line)
{
    char answer[200000];

    char request[2000];
    snprintf(request, sizeof(request),
        "%s HTTP/1.1\r\nHost: localhost:8080\r\n\r\n",
        request_line
    );

    int bytes = http_request(request, answer, sizeof(answer));
    TEST_ASSERT_TRUE_MESSAGE(bytes > 0, "Server did not answer");

    return parse_http_response(answer);
}

http_response_t make_request_and_parse_http_version_1_0(
    const char *request_line,
    int keep_alive   /* 1 = keep-alive, 0 = close */
)
{
    char answer[200000];
    char request[2000];

    if (keep_alive)
    {
        snprintf(request, sizeof(request),
            "%s HTTP/1.0\r\n"
            "Host: localhost:8080\r\n"
            "Connection: keep-alive\r\n"
            "\r\n",
            request_line
        );
    }
    else
    {
        snprintf(request, sizeof(request),
            "%s HTTP/1.0\r\n"
            "Host: localhost:8080\r\n"
            "Connection: close\r\n"
            "\r\n",
            request_line
        );
    }

    int bytes = http_request(request, answer, sizeof(answer));
    TEST_ASSERT_TRUE_MESSAGE(bytes > 0, "Server did not answer");

    return parse_http_response(answer);
}


// Helper pour envoyer un POST
http_response_t post_request(const char *path, const char *body)
{
    char request[2048];
    int body_len = strlen(body);

    snprintf(request, sizeof(request),
         "POST %s HTTP/1.1\r\n"
         "Host: localhost:8080\r\n"
         "Content-Length: %d\r\n"
         "Content-Type: text/plain\r\n"
         "\r\n"
         "%s",
         path, body_len, body);

    char response[8192];
    http_request(request, response, sizeof(response));

    return parse_http_response(response);
}




static int parse_status_code(const char *response)
{
    int status = 0;
    sscanf(response, "http/%*s %d", &status);
    return status;
}

static size_t parse_content_length(const char *headers)
{
    const char *cl = strstr(headers, "content-length:");
    if (!cl)
        return 0;

    cl += strlen("content-length:");
    while (*cl == ' ')
        cl++;

    return (size_t)atoi(cl);
}

http_response_stream_t parse_http_response_stream(const char *response)
{
    http_response_stream_t res;
    memset(&res, 0, sizeof(res));

    /* ================= status ================= */
    res.status = parse_status_code(response);
    
	/* ================= headers ================= */

    const char *header_end = strstr(response, "\r\n\r\n");
    if (!header_end)
        return res;

    size_t header_len = header_end - response + 4;

    /* ================= body ================= */

    size_t content_length = parse_content_length(response);

    if (content_length > 0)
    {
        res.body = (char *)malloc(content_length);
        if (!res.body)
            return res;

        memcpy(res.body, response + header_len, content_length);
        res.body_len = content_length;
    }
    else
    {
        res.body = NULL;
        res.body_len = 0;
    }

    return res;
}

void assert_header_contains(const char *headers, const char *needle)
{
    TEST_ASSERT(headers != NULL);
    TEST_ASSERT(strstr(headers, needle) != NULL);
}

void assert_header_not_contains(const char *headers, const char *needle)
{
    TEST_ASSERT(headers != NULL);
    TEST_ASSERT(strstr(headers, needle) == NULL);
}

void assert_connection_closed(int fd)
{
    char buf[1];
    ssize_t r = recv(fd, buf, sizeof(buf), MSG_DONTWAIT);

    // soit EOF, soit erreur EPIPE / ECONNRESET
    TEST_ASSERT(r == 0 || (r < 0 && errno != EAGAIN));
}

