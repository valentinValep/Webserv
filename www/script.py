#!/usr/bin/env python3

# print("Content-type: text/html")
print()

print("<html><head>")
print("<title>Script CGI en Python</title>")
print("</head><body>")

print("<h1>Formulaire CGI en Python</h1>")
print("<form method='post' action='script.py'>")
print("<p>Nom : <input type='text' name='nom' /></p>")
print("<p>Prenom : <input type='text' name='prenom' /></p>")
print("<p><input type='submit' value='Envoyer' /></p>")
print("</form>")

import cgi
form = cgi.FieldStorage()

if "nom" in form:
	nom = form.getvalue("nom")
	print("<h2>Bonjour, " + nom + "!</h2>")

print("</body></html>")