# Webserv

WebServ is an HTTP server project developed as part of the 42 school common core curriculum. It is implemented in C++98 and is designed to handle HTTP requests similar to NGINX, allowing users to serve static files or dynamic content through CGI.

## Features
- HTTP server capable of handling GET, POST, and DELETE methods
- Supports serving static files and dynamic content via CGI
- Uses the epoll function to handle multiple client connections concurrently
- Implements error codes to handle bad requests

### HTTP Protocol Overview
The Hypertext Transfer Protocol (HTTP) is an application layer protocol used for communication between web browsers and servers. It operates over TCP/IP and defines how messages are formatted and transmitted, as well as what actions web servers and browsers should take in response to various commands.

### Common Gateway Interface (CGI)
CGI is a standard protocol for interfacing external applications with web servers. It allows a web server to execute programs on the server and return dynamic content to the client. In the context of WebServ, CGI is used to generate dynamic content in response to HTTP requests.

## UML Class Diagram

Below is a visual representation of the structure and relationships between the classes in our WebServ project, presented through an UML Class Diagram:

![Class Diagram](https://github.com/valentinValep/Webserv/blob/main/img/uml_webserv.png)

## Usage

1. Clone or download the repository.

2. Navigate to the project directory:
```bash
cd Webserv
```

3. Compile the project using make:
```bash
make
```

4. Create static files and configure the server according to your requirements. An example configuration file and site directory is provided in the repository.

5. Launch the program:
```bash
./Webserv <optionnal: path to configuration file>
```




