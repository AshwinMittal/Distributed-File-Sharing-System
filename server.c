#include "header.h"
buildSocket(int *mode, char *port)
{
    int opt = 1;
    struct sockaddr_in server_addr, client_addr;
    int i, masterSock_fd, new_conn, max_sd, sd, valread;
    int sockaddr_len = sizeof(client_addr);
    char data[MAX_DATA], command[MAX_DATA];
    int data_len;

    struct hostent *he;
    struct in_addr ipv4addr;

    //set of socket descriptors
    fd_set active_fdset, read_fdset;
    struct connections connArr[5];

    //initialize all client_socket[] to 0
    for (i = 1; i < MAX_CLIENTS; i++)
    {
        connArr[i].id = 0;
    }

    //create socket
    if((masterSock_fd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
    {
        perror("ERROR: Creating Socket");
        exit(ERROR);
    }

    //set master socket to allow multiple connections
    if(setsockopt(masterSock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("ERROR: Set Socket");
        exit(ERROR);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&server_addr.sin_zero, 8);

    if(mode==1)
    {
        connArr[0].sock_fd = masterSock_fd;
        connArr[0].id = 1;
        strcpy(connArr[0].hostname, SERVER_NAME);
        strcpy(connArr[0].ipaddr, SERVER_IP);
        connArr[0].port = atoi(port);
    }

    if((bind(masterSock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) == ERROR)
    {
        perror("ERROR: Bind");
        exit(ERROR);
    }

    if((listen(masterSock_fd, MAX_CLIENTS)) == ERROR)
    {
        perror("ERROR: Listen");
        exit(ERROR);
    }

    //accept the incoming connection
    puts("Select one of the following commands (case sensitive):");
    help(mode);

    while(1)
    {
        FD_ZERO(&read_fdset);
        FD_SET(0, &read_fdset);
        FD_SET(masterSock_fd, &read_fdset);
        max_sd = masterSock_fd;

        for (i=0; i<MAX_CLIENTS; i++)
        {
            FD_SET(connArr[i].sock_fd, &read_fdset);
            max_sd = (max_sd>connArr[i].sock_fd)?max_sd:connArr[i].sock_fd;
        }

        if (select(max_sd+1, &read_fdset, NULL, NULL, NULL) < 0)
        {
            perror("ERROR: Select");
            //exit(EXIT_FAILURE);
        }

        if(FD_ISSET(masterSock_fd, &read_fdset))
        {
            //handle new connections
            new_conn = accept(masterSock_fd, (struct sockaddr *)&client_addr, (socklen_t*)&sockaddr_len);
            if ( new_conn == ERROR)
            {
                perror("ERROR: Accept");
                //exit(ERROR);
            }
            else
            {
                //inform user of socket number - used in send and receive commands
                printf("New Client having socketFD %d and IP %s has been connected to port no. %d\n", new_conn, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                //welcome msg
                char *message = "msg|-|Welcome!\r\n";

                if(send(new_conn, message, strlen(message), 0) != strlen(message) )
                {
                    perror("ERROR: Sending welcome msg");
                }
                else
                {
                    //puts("Welcome message sent successfully");
                }

                //add new socket to array of sockets
                int j;
                for (j = 1; j < MAX_CLIENTS; j++)
                {
                    //if position is empty
                    if(connArr[j].sock_fd == 0)
                    {
                        inet_pton(AF_INET, inet_ntoa(client_addr.sin_addr), &ipv4addr);
                        he = gethostbyaddr(&ipv4addr, sizeof(ipv4addr), AF_INET);

                        connArr[j].sock_fd = new_conn;
                        connArr[j].id = j+1;
                        strcpy(connArr[j].hostname, he->h_name);
                        strcpy(connArr[j].ipaddr, inet_ntoa(client_addr.sin_addr));
                        connArr[j].port = ntohs(client_addr.sin_port);
                        printf("Adding new socket descriptor %d to list of sockets at position %d\n", connArr[j].sock_fd, j+1);
                        break;
                    }
                }
                //sendList();
            }
        }

        for (i=0; i<MAX_CLIENTS; i++)
        {
            //Check closing
            sd = connArr[i].sock_fd;
            if(FD_ISSET(sd, &read_fdset) && sd!=0)
            {
                valread = recv(sd, data, MAX_DATA-1, 0);
                if(valread == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd, (struct sockaddr*)&client_addr, (socklen_t*)&sockaddr_len);
                    printf("Host having socketFD as %d and IP %s, disconnected from port no. %d\n", sd, inet_ntoa(client_addr.sin_addr) , connArr[i].port);

                    //Close the socket and mark as 0 in list for reuse
                    close(sd);
                    connArr[i].sock_fd = 0;
                    break;
                }
                else
                {
                    data[valread] = '\0';
                    char* t;
                    t = strtok(data,"|-|");
                    char *t1 = t;
                    t = strtok('\0',"|-|");
                    char *t2 = t;

                    if(strcmp(t1,"msg")==0)
                    {
                        printf("%s\n", t2);
                    }
                    else if(strcmp(t1,"port")==0)
                    {
                        set_port(connArr, sd, t2);
                    }
                    else if(strcmp(t1,"terminate")==0)
                    {
                        if(mode==2)
                        {
                            int j;
                            for (j = 1; j < MAX_CLIENTS; j++)
                            {
                                if(strcmp(t2,connArr[j].ipaddr)==0)
                                {
                                    getpeername(connArr[j].sock_fd, (struct sockaddr*)&client_addr, (socklen_t*)&sockaddr_len);
                                    printf("Host having socketFD as %d and IP %s, disconnected from port no. %d\n", sd, inet_ntoa(client_addr.sin_addr) , connArr[j].port);

                                    //Close the socket and mark as 0 in list for reuse
                                    close(connArr[j].sock_fd);
                                    connArr[j].sock_fd = 0;
                                    break;
                                }
                            }
                        }
                    }
                    else if(strcmp(t1,"quit")==0)
                    {
                        int j;
                        printf("%s\n",t2);
                        for (j = 1; j < MAX_CLIENTS; j++)
                        {
                            if(strcmp(t2,connArr[j].ipaddr)==0)
                            {
                                getpeername(connArr[j].sock_fd, (struct sockaddr*)&client_addr, (socklen_t*)&sockaddr_len);
                                printf("Host having socketFD as %d and IP %s, disconnected from port no. %d\n", sd, inet_ntoa(client_addr.sin_addr) , connArr[j].port);

                                //Close the socket and mark as 0 in list for reuse
                                close(connArr[j].sock_fd);
                                connArr[j].sock_fd = 0;
                                break;
                            }
                        }
                    }
                    else if(strcmp(t1,"get")==0)
                    {
                        if(mode==2)
                        {
                            int j;
                            char* tok;
                            tok = strtok(t2,"~");
                            char *tok1 = tok;
                            tok = strtok('\0',"~");
                            char *tok2 = tok;

                            for (j = 1; j < MAX_CLIENTS; j++)
                            {
                                if(strcmp(tok1,connArr[j].ipaddr)==0)
                                {
                                    sendfile(connArr[j].sock_fd, tok2);
                                    break;
                                }
                            }
                        }
                    }
                    else if(strcmp(t1,"file")==0)
                    {
                        if(mode==2)
                        {

                            char* tok;
                            tok = strtok(t2,"~");
                            char *tok1 = tok;
                            tok = strtok('\0',"~");
                            char *tok2 = tok;

                            FILE *fp;
                            fp = fopen(tok1, "w+");
                            fwrite(tok2, strlen(tok2) + 1, 1, fp);

                            fclose(fp);
                            //puts("File downloaded");
                            printf("%s has been downloaded from %s\n",tok1,connArr[i].hostname);
                        }
                    }
                    t='\0';
                    t1='\0';
                    t2='\0';
                }
            }
        }

        if(FD_ISSET(0, &read_fdset))
        {
            gets(command);
            char* t;
            t = strtok(command," ");
            char *t1 = t;
            t = strtok('\0'," ");
            char *t2 = t;
            t = strtok('\0'," ");
            char *t3 = t;

            if(strcmp("HELP",command)==0)
            {
                help(mode);
            }
            else if(strcmp("CREATOR",command)==0)
            {
                creator();
            }
            else if(strcmp("DISPLAY",command)==0)
            {
                char hostname[128];
                gethostname(hostname, sizeof(hostname));
                he = gethostbyname(hostname);
                display(inet_ntoa(*(struct in_addr*)he->h_addr) , port);
            }
            else if(strcmp("REGISTER",t1)==0)
            {
                if(strcmp(t2,SERVER_IP)!=0){
                    printf("\nERROR: Invalid Server IP Address.\n");
                    break;
                }
                else if(mode==2)
                {
                    if((sd = registerClient(port, t2, t3))>0)
                    {
                        connArr[0].sock_fd = sd;
                        connArr[0].id = 1;
                        strcpy(connArr[0].hostname, SERVER_NAME);
                        strcpy(connArr[0].ipaddr, SERVER_IP);
                        connArr[0].port = atoi(t3);

                        char buff2[20] = "port|-|";
                        strncat(buff2,port,20);
                        if(send(sd, buff2, strlen(buff2), 0) != strlen(buff2) )
                        {
                            perror("ERROR: Sending port");
                        }
                    }
                }
                else
                {
                    printf("\nERROR: Invalid Command. Type HELP to see the valid commands.\n");
                }
            }
            else if(strcmp("CONNECT",t1)==0)
            {
                if(mode==2)
                {
                    if((sd = connectPeer(port, t2, t3))>0)
                    {
                        int j;
                        for (j = 1; j < MAX_CLIENTS; j++)
                        {
                            if(connArr[j].sock_fd == 0)
                            {
                                inet_pton(AF_INET, t2, &ipv4addr);
                                he = gethostbyaddr(&ipv4addr, sizeof(ipv4addr), AF_INET);

                                connArr[j].sock_fd = sd;
                                connArr[j].id = j+1;
                                strcpy(connArr[j].hostname, he->h_name);
                                strcpy(connArr[j].ipaddr, t2);
                                connArr[j].port = atoi(t3);
                                FD_SET(sd, &read_fdset);
                                if((max_sd<sd))
                                {
                                    max_sd = sd;
                                }
                                break;
                            }
                        }

                        char buff2[20] = "port|-|";
                        strncat(buff2,port,20);
                        if(send(sd, buff2, strlen(buff2), 0) != strlen(buff2) )
                        {
                            perror("ERROR: Sending port");
                        }
                    }
                }
                else
                {
                    printf("\nERROR: Invalid Command. Type HELP to see the valid commands.\n");
                }
            }
            else if(strcmp("LIST",command)==0)
            {
                list(connArr);
            }
            else if(strcmp("TERMINATE",t1)==0)
            {
                if(mode==2)
                {
                    int j;
                    for (j = 1; j < MAX_CLIENTS; j++)
                    {
                        if(connArr[j].id == atoi(t2))
                        {
                            getpeername(connArr[j].sock_fd, (struct sockaddr*)&client_addr, (socklen_t*)&sockaddr_len);
                            printf("You have terminated the connection with host having socketFD %d and IP %s, from port no. %d\n", connArr[j].sock_fd, inet_ntoa(client_addr.sin_addr) , ntohs(client_addr.sin_port));

                            char buff2[50] = "terminate|-|";
                            strncat(buff2,connArr[j].ipaddr,50);

                            if(send(connArr[j].sock_fd, buff2, strlen(buff2), 0) != strlen(buff2) )
                            {
                                perror("ERROR: Sending terminate request");
                            }
                            //Close the socket and mark as 0 in list for reuse
                            close(connArr[j].sock_fd);
                            connArr[j].sock_fd = 0;
                            break;
                        }
                    }
                }
                else
                {
                    printf("\nERROR: Invalid Command. Type HELP to see the valid commands.\n");
                }
            }
            else if(strcmp("QUIT",command)==0)
            {
                if(mode==2)
                {
                    char hostname[128];
                    gethostname(hostname, sizeof(hostname));
                    he = gethostbyname(hostname);
                    char ip[50];
                    strcpy(ip, inet_ntoa(*(struct in_addr*)he->h_addr));

                    int j;
                    for (j = 0; j < MAX_CLIENTS; j++)
                    {
                        if(connArr[j].sock_fd > 0)
                        {
                            char buff2[50] = "quit|-|";
                            strncat(buff2,ip,50);

                            if(send(connArr[j].sock_fd, buff2, strlen(buff2), 0) != strlen(buff2) )
                            {
                                perror("ERROR: Sending quit request");
                            }
                            //Close the socket and mark as 0 in list for reuse
                            close(connArr[j].sock_fd);
                            connArr[j].sock_fd = 0;
                        }
                    }
                    printf("\nYou have been disconnected from the server and all other peers.\n");
                }
                else
                {
                    printf("\nERROR: Invalid Command. Type HELP to see the valid commands.\n");
                }
            }
            else if(strcmp("GET",t1)==0)
            {
                if(mode==2)
                {
                    if(atoi(t2)==1)
                    {
                        printf("\nERROR: You can not request file form the server.\n");
                    }
                    else
                    {
                        int j;
                        for (j = 1; j < MAX_CLIENTS; j++)
                        {
                            if(connArr[j].id == atoi(t2))
                            {
                                char hostname[128];
                                gethostname(hostname, sizeof(hostname));
                                he = gethostbyname(hostname);

                                char buff2[10] = "get|-|";
                                strncat(buff2,inet_ntoa(*(struct in_addr*)he->h_addr),20);

                                char buff3[5] = "~";
                                strncat(buff3,t3,50);
                                strncat(buff2,buff3,60);

                                if(send(connArr[j].sock_fd, buff2, strlen(buff2), 0) != strlen(buff2) )
                                {
                                    perror("ERROR: Request file");
                                }
                                else
                                {
                                    puts("Request sent");
                                }
                                break;
                            }
                        }
                    }
                }
                else
                {
                    printf("\nERROR: Invalid Command. Type HELP to see the valid commands.\n");
                }
            }
            else if(strcmp("PUT",t1)==0)
            {
                if(mode==2)
                {
                    if(atoi(t2)==1)
                    {
                        printf("\nERROR: You can not send file to the server.\n");
                    }
                    else
                    {
                        int j;
                        for (j = 1; j < MAX_CLIENTS; j++)
                        {
                            if(connArr[j].id == atoi(t2))
                            {
                                sendfile(connArr[j].sock_fd, t3);
                                break;
                            }
                        }
                    }
                }
                else
                {
                    printf("\nERROR: Invalid Command. Type HELP to see the valid commands.\n");
                }
            }
            else
            {
                printf("\nERROR: Invalid Command. Type HELP to see the valid commands.\n");
            }
        }
    }
}
