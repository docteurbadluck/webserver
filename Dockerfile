FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    python3 \
    curl \
    netcat-openbsd \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

# Fix paths in config and test files to match container path
RUN cp config.test.original webserv/config.conf \
    && sed -i 's|/home/docteurbadluck/Desktop/rearanged_webserver/|/app/|g' webserv/config.conf \
    && sed -i 's|ip = 127.0.0.2|ip = 0.0.0.0|g' webserv/config.conf \
    && sed -i 's|ip = 127.0.0.1|ip = 0.0.0.0|g' webserv/config.conf

# Fix paths in test_delete
RUN sed -i 's|/home/docteurbadluck/Desktop/rearanged_webserver/|/app/|g' test_delete.original.c \
    && cp test_delete.original.c testeur_webserver/test_delete.c

# Setup forbidden directory for tests
RUN mkdir -p www/forbidden_directory \
    && touch www/forbidden_directory/index.html \
    && chmod 000 www/forbidden_directory/index.html

# Create selfie upload directory
RUN mkdir -p www/upload/selfies

# Make CGI scripts executable
RUN chmod +x www/CGI/test.py www/CGI/hello.py www/CGI/contact.py www/CGI/gallery.py

# Build webserver (clean first to remove any host .o files)
RUN cd webserv && make fclean && make

# Build tester
RUN cd testeur_webserver && make

EXPOSE 8080 8081

CMD ["./webserv/webserv", "webserv/config.conf"]
