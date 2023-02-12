# http-server

## Client-Server over Socket Programming

Basically, the server runs a server process over TCP while each client creates a client process to connect to this
server process. Please run the server first. After that, please run a client, and you will be prompted to
input a message and then the message will be sent to the server after clicking ”Enter”. Upon receiving
the message, the server will send the message back, which will be displayed on the client terminal. The
server will show the message on its terminal, too.

Please run the server first. The client will send [message] to the server and wait for the response
message from the server. After the response message is received, the client will close the connection
(after a while, explained next). The server will print out the received messages on its terminal in the
following format and send the same message back to the client as the response message:
message-from-client: [client_ip] [client_port] [message] where [client_ip] and [client_port] are the IP address and Port Number used by the client.
The serverMul.c supports multiple clients using Pthreads.

## HTTP/1.1

This simple web server implements the following features:
* When the client sends GET to request for one .html which exists, it responds “200 OK ” and
return this .html file.
* When the client sends GET to request for one .html which doesn’t exist, it sends the response
“404 Not found ”.
* When the client sends GET with the wrong format (e.g. URL String Syntax Error), it sends
the response “400 Bad Request ”.
* When the client sends GET with a different HTTP version, it sends the response “505 HTTP
Version Not Supported ”.

Before you run the server code, please create a folder named www in the folder containing your server 
program and place all the web objects in the www folder.

## HTTP/2.0
* Methods, status codes, most header fields unchanged from HTTP 1.1
* Increased flexibility at server in sending objects to client
* Transmission order of requested objects based on client-specified object priority (not necessarily FCFS)
* Divides objects into frames, schedules frames to mitigate head-of-line (HOL) blocking