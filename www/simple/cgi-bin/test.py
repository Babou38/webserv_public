#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/html")
print()  # Séparation obligatoire entre headers et body

print("<html><body>")
print("<h1>CGI Test</h1>")

try:
    length = int(os.environ.get('CONTENT_LENGTH', 0))
    data = sys.stdin.read(length)
    print(f"<p>Reçu: {data}</p>")
except Exception as e:
    print(f"<p>Erreur: {e}</p>")

print("</body></html>")
