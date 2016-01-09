SETUP:
The program starts by creating an object called "server" using the MakeFile named make.txt.
Start the program by calling this object on command line and passing 2 parameters. First, either letter "s" for this server to act as a Server or letter "c"
for this server to act as a Client. Second, is the listening port this server.

1. make -f make.txt
2. ./server s 4322 OR ./server c 4122


COMMANDS:
The process accepts an incoming connection at all times and at the same times offers following command options to the user:

1. HELP
#Display information about the available user command options.

2. DISPLAY
#Display the IP address of this process, and the port on which this process is listening for incoming connections.

3. REGISTER <server IP> <port no>
#This command is used by the client to register itself with the server and to get the IP and listening port numbers of all other peers currently registered with the server.
NOTE: The REGISTER command works only on the client and not on the server. Registered clients should always maintain a live TCP connection with the server.

4. CONNECT <destination> <port no>
#This command is used to establish a connection between two registered clients. The command establishes a new TCP connection to the specified <destination> at the specified <port no>.

5. LIST
#Display a numbered list of all the connections this process is part of.

6. TERMINATE <connection id>
#This command will terminate the connection listed under the specified number when LIST is used to display all connections.

7. QUIT
#Close all connections and terminate this process.

8. GET <connection id> <file> 
#This command will download a file from one host specified in the command. 

9. PUT <connection id> <file name> 
#For example, ‘PUT 3 /local/Fall_2015/qiao/a.txt’ will put the file a.txt which is located in /local/Fall_2015/qiao/, to the host on the connection that has connection id 3. 
