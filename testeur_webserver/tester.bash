#!/bin/bash

SERVER="127.0.0.1:8080"

pass() { echo -e "[\e[32mPASS\e[0m] $1"; }
fail() { echo -e "[\e[31mFAIL\e[0m] $1"; }

test_keep_alive() {
    desc="$1"
    cmd="$2"
    expected="$3"

    output=$(eval "$cmd")
    if echo "$output" | grep -q "$expected"; then
        pass "$desc"
    else
        fail "$desc"
        echo "Output was:"
        echo "$output"
    fi
}

echo "=== Webserver Keep-Alive / Close Tests ==="

# 1. HTTP/1.1 default keep-alive
test_keep_alive "HTTP/1.1 default keep-alive" \
    "curl -v http://$SERVER/ 2>&1" \
    "left intact"

# 2. HTTP/1.1 with Connection: close
test_keep_alive "HTTP/1.1 with Connection: close" \
    "curl -v -H 'Connection: close' http://$SERVER/ 2>&1" \
    "Closing connection"

# 3. HTTP/1.0 default (close)
test_keep_alive "HTTP/1.0 default close" \
    "curl -v --http1.0 http://$SERVER/ 2>&1" \
    "Closing connection"

# 4. HTTP/1.0 with Connection: keep-alive
test_keep_alive "HTTP/1.0 with Connection: keep-alive" \
    "curl -v --http1.0 -H 'Connection: keep-alive' http://$SERVER/ 2>&1" \
    "left intact"

echo "=== Tests completed ==="

