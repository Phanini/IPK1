<!-- AUTHOR -->
Jakub Phan, xphanj00
2BIT FIT VUT

<!-- ABOUT THE PROJECT -->
This is the first project in Computer communication and sites (IPK) course that implements a HTTP server in C. 

<!-- USAGE -->
The server is compilable by Makefile that will create an executable hinfosvc file. Execute this file with an argument indicates what port will be used to listen to requests.

example:
./hinfosvc 12345

Server understands 3 request types that are sent by GET command. HOSTNAME, CPU-NAME and LOAD.

example:
GET http://servername:12345/hostname
GET http://servername:12345/cpu-name
GET http://servername:12345/load