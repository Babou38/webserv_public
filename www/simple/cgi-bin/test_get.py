# www/simple/cgi-bin/test_get.py
#!/usr/bin/env python3

import os

print("Content-Type: text/html")
print()

print("<html><body>")
print("<h1>Test GET CGI</h1>")

print(f"<p><b>Methode HTTP :</b> {os.environ.get('REQUEST_METHOD')}</p>")
print(f"<p><b>URI demandee :</b> {os.environ.get('SCRIPT_NAME')}</p>")
print(f"<p><b>Serveur :</b> {os.environ.get('SERVER_NAME')}:{os.environ.get('SERVER_PORT')}</p>")

print("</body></html>")

