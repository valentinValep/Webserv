#!/usr/bin/env python3
# HTML header, title and start of body
# print("Content-type: text/html")
import cgi, cgitb
import os

form = cgi.FieldStorage()

if "nom" in form and "prenom" in form:
	nom = form.getvalue('nom')
	prenom = form.getvalue('prenom')
	print("<p>nom: ", nom, "</p>")
	print("<p>prenom: ", prenom, "</p>")
else:
	print("""
<html><head>
<title>CGI Script in Python</title>
</head><body>

<h1>Python CGI Form</h1>
<form method='post' action='/script_form.py'>
<p>Name: <input type='text' name='nom' /></p>
<p>First Name: <input type='text' name='prenom' /></p>
<p><input type='submit' value='Submit' /></p>
</form>"""
)

print("</body></html>")
