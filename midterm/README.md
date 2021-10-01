create a client app called telnet-client, and a server app called telnet-server 

after client app is connected to the server, client and server can interact via the following commands
- pwd: display the current working directory from server 
- cd: change the server app working directory
- ls: list all the files/folders under server's current working directory
- quit: client quits, but server cannot quit. Another client app can be connected to server again 


================== client app requirements ==============================
1. client side can indicate the server ip and port
2. after entering a command, client will wait for server's response, and will display the result on the screen 
3. examples: 

$ ./telnet-client 127.0.0.1 43211
$ pwd
/home/danielhan/Documents/sockets/midterm
$ cd ..
$ pwd
/home/danielhan/Documents/sockets
$ ls
telnet-client.cc
telnet-client
telnet-server.cc
telnet-server
$ cd /home
$ pwd
/home
$ quit


$ ./telnet-client 127.0.0.1 43211
$ pwd
/home

$ quit


=================== server app requirements =================================
1. For now, we don't consider multiple client's connections. We only have one client connected 
2. server should return the commands results back to the client 
3. server shouldn't quit after client quits 


