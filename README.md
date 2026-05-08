*This project has been created as part of the 42 curriculum by jholterh, tdeliot.*

## Description

Webserv is a lightweight HTTP/1.1 web server written in C++98. It handles GET, POST, and DELETE requests, serves static files, supports CGI script execution, file uploads, directory listing, HTTP redirections, cookies and sessions, and multiple virtual server configurations. The server uses a single epoll instance for non-blocking I/O multiplexing.


https://github.com/user-attachments/assets/d8ee2fcd-5cd6-4fc4-a076-c93d83813041





https://github.com/user-attachments/assets/1eb89365-5086-4a0c-b59a-5b0b75e8bd69




https://github.com/user-attachments/assets/a130ae8e-42d6-42ad-b48d-a06b5c4c9cfe




## Instructions

### Compile

```bash
./RUN_ME_ONCE.sh
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
