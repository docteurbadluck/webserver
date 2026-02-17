#ifndef TEST_H
#define TEST_H

# include "unity.h"
# include "unity_internals.h"
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <errno.h>
# include <fcntl.h>
# include <unistd.h>  
             
typedef struct  
{               
    	int status_code;
    	char *headers;
    	char *body;  
} http_response_t; 

typedef struct http_response_stream_s
{
    int     status;
    char   *body;
    size_t  body_len;
} http_response_stream_t;



  
                  
http_response_t make_request_and_parse(const char *request_line);
int				http_request(const char *request, char *response, size_t max_len);
http_response_t	parse_http_response(char *raw);
http_response_t post_request(const char *path, const char *body);
http_response_t make_request_and_parse_http_version_1_0(const char *request_line,int keep_alive);

void test_get_200(void);
void test_get_200_big_size(void);
void test_get_403(void);
void test_get_404(void);
void test_get_405(void);
void test_get_500(void);
void test_get_binary(void);

void test_get_200_extra_big_size(void);

void test_post_200(void);
void test_post_403(void);
void test_post_404(void);
void test_post_405(void);
void test_post_413(void);
void test_post_500(void);
 
void test_delete_200(void);
void test_delete_404(void);
void test_delete_403(void);
void test_delete_405(void);
	


http_response_t make_request_and_parse_http_version_1_0(
    const char *request_line,
    int keep_alive
);

http_response_stream_t parse_http_response_stream(const char *response);
void assert_header_contains(const char *headers, const char *needle);
void assert_header_not_contains(const char *headers, const char *needle);
void assert_connection_closed(int fd);


void test_two_get_requests_in_one_tcp_packet(void);
void test_http10_close_after_first_request(void);
void test_five_get_requests_in_one_tcp_packet(void);


void test_redirect_301(void);
void test_redirect_302(void);
void test_redirect_wrong_name(void);

void test_http_1_0(void);
void test_http_1_1(void);





#endif             
