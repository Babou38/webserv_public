# www/simple/cgi-bin/test_post.py
#!/usr/bin/env python3

import os
import sys

print("Content-Type: text/html")
print()

print("<html><body>")
print("<h1>Test POST CGI</h1>")

try:
    length = int(os.environ.get("CONTENT_LENGTH", 0))
    body = sys.stdin.read(length)
    print(f"<p><b>Donnees POST :</b> {body}</p>")
    print(f"<p><b>Content-Type :</b> {os.environ.get('CONTENT_TYPE')}</p>")
except Exception as e:
    print(f"<p>Erreur : {e}</p>")

print("</body></html>")
