#!/usr/bin/env python3

import cgi, os
import cgitb; cgitb.enable()

form = cgi.FieldStorage()

# HTML header, title and start of body
# print("Content-type: text/html")
print()
print("<html><head>")
print("<title>CGI Script in Python</title>")
print("</head><body>")

print("<h1>Python CGI Form</h1>")
print("<form method='post' action='script.py' enctype='multipart/form-data'>")
print("<p>File: <input type='file' name='fichier' /></p>")
print("<p>Name: <input type='text' name='nom' /></p>")
print("<p>First Name: <input type='text' name='prenom' /></p>")
print("<p><input type='submit' value='Submit' /></p>")
print("</form>")

# Handling form data
if "nom" in form:
    nom = form.getvalue("nom")
    print("<h2>Hello, " + nom + "!</h2>")

# Handling file upload
if "fichier" in form:
    fileitem = form['fichier']
    if fileitem.filename:
        # strip leading path from file name to avoid directory traversal attacks
        fn = os.path.basename(fileitem.filename)
        open('/path/to/store/uploads/' + fn, 'wb').write(fileitem.file.read())
        message = 'The file "' + fn + '" was uploaded successfully'
    else:
        message = 'No file was uploaded'
    print("<p>{}</p>".format(message))

print("</body></html>")
