#!/usr/bin/env python3

import os
import sys
import urllib.parse

method = os.environ.get("REQUEST_METHOD", "GET")
query = os.environ.get("QUERY_STRING", "")

# Parse GET params
get_params = urllib.parse.parse_qs(query)

# Parse POST body
post_params = {}
if method == "POST":
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    if content_length > 0:
        body = sys.stdin.read(content_length)
        post_params = urllib.parse.parse_qs(body)

# Get the name from either POST or GET
name = ""
if "name" in post_params:
    name = post_params["name"][0]
elif "name" in get_params:
    name = get_params["name"][0]

# Build the page
print("Content-Type: text/html")
print("")

print("""<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>CGI Greeter</title>
<style>
  html, body { height: 100%%; margin: 0; font-family: system-ui, sans-serif; }
  body { display: grid; place-items: center; background: #0f172a; color: #e2e8f0; }
  .card {
    padding: 2.5rem 3rem; border-radius: 1.25rem;
    background: rgba(30,41,59,0.6); backdrop-filter: blur(6px);
    box-shadow: 0 10px 25px rgba(0,0,0,0.35); text-align: center;
    min-width: 320px;
  }
  h1 { margin: 0 0 1rem; font-size: 2rem; }
  .greeting { font-size: 1.5rem; margin: 1rem 0; color: #38bdf8; }
  form { margin: 1.5rem 0 0; }
  input[type=text] {
    padding: 0.6rem 1rem; border-radius: 0.5rem; border: 1px solid #334155;
    background: #1e293b; color: #e2e8f0; font-size: 1rem; width: 200px;
  }
  button {
    padding: 0.6rem 1.5rem; border-radius: 0.5rem; border: none;
    background: #2563eb; color: white; font-size: 1rem; cursor: pointer;
    margin-left: 0.5rem;
  }
  button:hover { background: #1d4ed8; }
  .info { margin-top: 1.5rem; font-size: 0.8rem; opacity: 0.5; }
</style>
</head>
<body>
<div class="card">
  <h1>CGI Greeter</h1>""")

if name:
    import html
    safe_name = html.escape(name)
    print('  <div class="greeting">Hello, {}! 👋</div>'.format(safe_name))
else:
    print('  <div class="greeting">Enter your name below</div>')

print("""  <form method="POST" action="/website_name/CGI/hello.py">
    <input type="text" name="name" placeholder="Your name..." autofocus>
    <button type="submit">Greet me!</button>
  </form>
  <div class="info">Method: {} | Python CGI running on webserv</div>
</div>
</body>
</html>""".format(method))
