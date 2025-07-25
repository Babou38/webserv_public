#!/usr/bin/env python3

import os
import sys

print()
print("<html><body>")
print("<h1>CGI Test sans Content-Type</h1>")
print(f"<p>Reçu (method: {os.environ.get('REQUEST_METHOD')}):</p>")
print("<ul>")
for key in sorted(os.environ):
    if key.startswith("HTTP_") or key in ["QUERY_STRING", "CONTENT_LENGTH", "CONTENT_TYPE"]:
        print(f"<li>{key} = {os.environ[key]}</li>")
print("</ul>")
print("</body></html>")
