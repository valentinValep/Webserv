#!/usr/bin/env python3
# HTML header, title and start of body
# print("Content-type: text/html")
print()
print("<html><head>")
print("<title>CGI Script in Python</title>")
print("</head><body>")

print("<h1>Python CGI Form</h1>")
print("<form method='post' action='/cgi/uploads' enctype='multipart/form-data'>")
print("<p>File: <input type='file' name='fichier' /></p>")
print("<p>Name: <input type='text' name='nom' /></p>")
print("<p>First Name: <input type='text' name='prenom' /></p>")
print("<p><input type='submit' value='Submit' /></p>")
print("</form>")
print("</body></html>")
