*This project has been created as part of the 42 curriculum by jholterh, tdeliot.*

## Description

Webserv is a lightweight HTTP/1.1 web server written in C++98. It handles GET, POST, and DELETE requests, serves static files, supports CGI script execution, file uploads, directory listing, HTTP redirections, cookies and sessions, and multiple virtual server configurations. The server uses a single epoll instance for non-blocking I/O multiplexing.

## Instructions

### Compile

```bash
cd webserv
make
```

### Run

```bash
./webserv config.conf
```

Then open `http://127.0.0.1:8080` in a browser.

### Run Tests

```bash
cd testeur_webserver
make
./testeur_webserver
```

## Resources

- [RFC 7230 - HTTP/1.1 Message Syntax and Routing](https://datatracker.ietf.org/doc/html/rfc7230)
- [RFC 7231 - HTTP/1.1 Semantics and Content](https://datatracker.ietf.org/doc/html/rfc7231)
- [RFC 3875 - The Common Gateway Interface (CGI) Version 1.1](https://datatracker.ietf.org/doc/html/rfc3875)
- [epoll(7) - Linux I/O event notification](https://man7.org/linux/man-pages/man7/epoll.7.html)
- [NGINX beginners guide](https://nginx.org/en/docs/beginners_guide.html)

### AI Usage

AI tools (Claude) were used during development for:
- Debugging HTTP parsing edge cases and CGI environment variable setup
- Reviewing code for compliance with the 42 subject requirements
- Generating test cases for chunked transfer encoding
