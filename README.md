# Author
Jakub Phan, xphanj00<br />
2BIT FIT VUT

## ABOUT THE PROJECT 
This is the first project in Computer Communication and Networks (IPK) course that implements a HTTP server in C. 

## USAGE
The server is compilable by Makefile that will create an executable hinfosvc file. Execute this file with an argument indicates what port (range 1-65535) will be used to listen to requests.

example:<br />
```
./hinfosvc [port_number]
```

Server understands 3 request types that are sent by GET command. HOSTNAME, CPU-NAME and LOAD.
(use http://localhost:[port_number] for testing)

example:<br />
```
GET http://servername:12345/hostname
GET http://servername:12345/cpu-name
GET http://servername:12345/load
```
## Evaluation
| Basic tests | Unsupported call tests | Wrong call tests | Documentation |
|:-----------:|:----------------------:|:----------------:|:-------------:|
|    10/10    |           5/5          |        2/2       |      3/3      |
