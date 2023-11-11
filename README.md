# Webserv


<h2>TO DO</h2>

- [ ] Handle more than one server (based on config file, set up as many server_fd and corresponding pollfds or client sockets as there are servers)

- [ ] The read loop, implement a loop to read from the socket in chunks (e.g., in a buffer of 1024 bytes). After each read: (i) append the data to a request buffer, (ii) check if the buffer contains the end of the request (e.g., an empty line for headers, the end of the content based on Content-Length, the last chunk in chunked encoding, etc.) (iii) If the end of the request is detected, process the request (iv) If not, continue reading from the socket)

- [ ] Store the above buffer and loop until received fully (end token reached). Then parse the request, process it (GET, DELETE, POST), and compose the reponse. Then set flag to POLLOUT and send once ready


<h2> BASICS </h2>

<details><summary><h6>What is a web server ?</h6></summary>

From a hardware point of view, anything with an internet connection can be a web server e.g a laptop, anything… It’s not about hardware (even if some are more suited), it’s a piece of software that serves web content. Being a server is a role. 

An HTTP web server is a software application that listens for and responds to HTTP requests from clients (such as web browsers). The main purpose of a web server is to host web content and make it available to users over the internet.

Let’s look at the 6 things (roughly) that a web server does : 

1. LISTENS
2. ON A PORT
3. FOR A REQUEST
4. Send via a TRANSPORT PROTOCOL
5. Returns a RESPONSE
6. Containing the requested RESOURCES

Once a web server has been set up, it justs sits there i.e listens until something comes around. If nothing comes around it’s idle. The web server listens to a network port provided by the operating system that the web server is running on. 

HTTP Message Format

```
start-line CRLF
Headers CRLF
CRLF(end of headers)
[message-body]

CRLF are Carriage Return and Line Feed (\r\n), which is just a new line.
```


</details>


<details><summary><h6>What is HTTP, Hyper Text Transfer Protocol ?</h6></summary>

Le protocole HTTP est un protocole de communication spécifiquement pensé pour le web. 90% des échanges web se font via ce protocole. Son rôle est de définir comment les messages doivent être structurés et comment sont organisés les échanges de données entre navigateurs et serveurs web. Il permet d'échanger notamment le contenu des sites web comme les textes images vidéos code source etc. Donc globalement tout ce qui est affiché sur le navigateur. 

Le protocole HTTP est considéré comme étant stateless, c’est à dire qu’il ne conserve pas l'état des requêtes, le serveur ne garde pas de mémoire de l’historique des interactions avec un client. Chaque requête HTTP est différente. C’est là que les cookies interviennent pour enregistrer et suivre l'état et l’historique de votre navigation sur le web (d’autres outils existent pour compenser ce manque d'état dans le protocole http). 

Le concept d'hypertexte implique que les pages web renvoient les utilisateurs sur d’autres pages via des liens cliquables. Ces liens comportent une URL (Uniform Resource Locator), une chaîne de caractères utilisée pour spécifier l'adresse d'une ressource spécifique sur internet. Une URL contient les informations suivantes : 

* Protocole
* Nom de domaine (adresse du serveur ou l’adresse est hébergée)
* Chemin (d'accès à la ressource sur le serveur)
* Nom du fichier 
* Parametres

  ![Capture d’écran   2023-10-30 à 11 24 57](https://github.com/chmadran/Webserv/assets/113340699/7551c31c-8af0-4495-9ce5-af26636fa6dc)

Lorsqu’on clique sur un client ou qu’on entre une URL, votre navigateur envoie une requête au serveur web concerne, la requête HTTP détermine le type d’action qu’on souhaite effectuer (GET pour obtenir des donees, PUT ou POST pour envoyer des données de formulaire ou télécharger des fichiers, DELETE etc).

A REQUEST
![image](https://github.com/chmadran/Webserv/assets/113340699/22c64d00-d991-44c0-b177-57c37999df20)

A RESPONSE 
![image](https://github.com/chmadran/Webserv/assets/113340699/948b4503-4334-4a84-a232-b6043ba1e30c)


</details>

<details><summary><h6>What is a socket ?</h6></summary>

**Open System Interconnection (OSI) Models - 7 Layers**

![Screenshot from 2023-10-30 14-05-08](https://github.com/chmadran/Webserv/assets/113340699/c646617f-d29b-464a-896a-ecaf22d7bc16)

In order for a connection to take place between two devices, we know the data must be sent from the application layer and descend down to the physical layer and then to ascend from the physical layer to the application layer in order to be received by the other device.  

While the OSI layers are the same in the client and in the server, **the roles and the tasks that these two devices perform will differ** in a client / server architecture. That's where we will label one of the two devices to be a client and the other device to be a server. They perform different roles and tasks. 

The connection is always between two devices (even if a server can communicate with more than one client and vice versa) and each side uses its own IP and port number. The server is continually waiting for incoming connections. This is called **listening** which is always done on a certain IP and port number. The loopback address 127.0.0.1 refers to the current address and is reserved. It can be used to test both client and server on a single machine. 

Port numbers (16 bit address) can be any integer between 1 and 65 535. Port 1..1023 are described as well known ports and are reserved for specific applications. It’s recommended to choose a number over 1024. Each machine on the network has its own unique IP address and then at each IP address there will be ports for data to be sent from and received at. 

Now we can try and undergo a process of binding that IP address and port number to a dedicated socket : one for the server listening for clients attempting to connect to that socket and the client itself will create a socket which will specify the details of the server, the IP address and the port number that it wants to connect to. We have a two way reciprocal connection here : the server is in listening mode willing to accept requests and the client is active in sending out requests to those details (IP address and port number). 

Once the connection has been approved, the socket facilitates the connection and allows for data to be sent, it’s a “pipe between two computers on a network through which data flows.”. Sockets are two-way, data can be both sent and received on a socket. There are two types of sockets : 

* Streaming socket (uses the TCP protocol)
* Datagram socket (uses the UCP protocol)

When we create a socket, it is yet to be bound to an IP or a port number. The status in this case is **unbound**. When the IP address and port number have been set, the socket is bound. Once bound, the socket can be set to a listening state for incoming connections on the port. The client then makes a request to connect to the server’s port number. Once the server request has been acknowledged and accepted by the server, we dont just connect the two sockets but the server duplicates the socket to enable the connection between the server and the client and maintains the original socket so its ready to listen to other clients. 

The server can simultaneously maintain its connection with the client  while it can keep on listening out to other clients attempting to connect to that socket. However, creating a socket on the server does not mean that all clients' requests come into that socket. Since connections are managed 1:1, one socket is used only to detect connection requests from new clients. When detected, an individual connection (socket) is created with each client and messages are exchanged. It is good for variable names and collaboration to clearly agree on names for the former and the latter. The former is a rendezvous socket, but we called it a server socket for convenience, and the latter uses client fd/socket/connection interchangeably.


![Screenshot from 2023-10-30 14-36-07](https://github.com/chmadran/Webserv/assets/113340699/ad28d0c7-18ab-44f1-942c-ff6f5c2467aa)


</details>

<h2> NOTES & RESSOURCES </h2>

<details><summary><h4>USEFUL RESSOURCES</h4></summary>

* [Maxime's](https://github.com/maxime-42/webserv/tree/main/srcs) Webserv 
* [Lecture on Asynchronous I/O](https://www.youtube.com/watch?v=tAGF0T2cXRA)
* [Webserv TUTORIAL](https://42seoul.gitbook.io/webserv/) so goood for a braod understanding of the main topics
* [HTML, an Overview](https://developer.mozilla.org/en-US/docs/Web/HTTP/Overview)
* [HTML, an other Overview](https://feel5ny.github.io/2019/08/03/HTTP_001/)
*  [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/split/index.html) in case of emergency
* [RFC](http://www.rfc.fr/rfc/fr/rfc2616.pdf) - initially it is RFC 2616 but later replaced by RFC 7230, RFC 7231, RFC 7232, RFC 7233, RFC 7234, RFC 7235
* [The ultime guide to build a web server](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)
* To understand [sockets](https://www.youtube.com/watch?v=gntyAFoZp-E&t=1250s)
* Kaydoo's [README](https://github.com/Kaydooo/Webserv_42)
  
</details>

<details><summary><h4>RANDOM VOCABULARY NOTES</h4></summary>
  
* **A Request for Comments (RFC)**, in the context of Internet governance, is a type of publication from the Internet Engineering Task Force (IETF) and the Internet Society(ISOC), the principal technical development and standards-setting bodies for the Internet.

* **Domain Name Server** or DNS is the phonebook of domain name=IP Address.

* **Whats a fully static website?**
A fully static website is a website that consists only of HTML, CSS, JavaScript, images, and other client-side resources, and does not require any server-side processing or dynamic content generation.     

In a fully static website:       
     
HTML: Contains the structure and content of the web pages.    
CSS: Defines the styling and layout of the web pages.    
JavaScript: Provides interactivity and dynamic behavior on the client side.    
Images and other resources: Enhance the visual elements of the website.    

There is no server-side processing or database interaction involved. All the content is pre-defined and stored as files, and the web server simply serves these files to the client upon request.    
</details>

<h2> TESTS / OTHER </h2>

* To see the client request being made/sent to your server, head to Inspect > Nextwork 
![Screenshot from 2023-11-01 14-12-44](https://github.com/chmadran/Webserv/assets/113340699/627d7a73-b642-42cf-b566-a54258792e2f)
