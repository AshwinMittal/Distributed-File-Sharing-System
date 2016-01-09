#include "header.h"
help(int *mode)
{
    puts(" ");
    puts("HELP ==> Displays information about available commands");
    puts("CREATOR ==> Displays Full Name, UBIT Name and UB Email");
    puts("DISPLAY ==> Displays the IP Address and Listening Port No.");
    if(mode==1)
    {
        puts("LIST ==> Displays numbered list of all the registered connections");
    }
    if(mode==2)
    {
        puts("REGISTER <serverIP> <Port No.> ==> Registers a client with the server");
        puts("CONNECT <destination> <Port No.> ==> Establish connection between registered clients. [destination] = IP Addr. of remote host; [Port No.] = Listening port of the host");
        puts("LIST ==> Displays numbered list of all the registered connections");
        puts("TERMINATE <connection id> ==> Terminate connection at the listed id");
        puts("QUIT ==> Close all connections");
        puts("GET <connection id> <file> ==> Downloads file from specified host");
        puts("PUT <connection id> <file> ==> Puts the file to host at specified connection id");
        puts("SYNC ==> Will download all the shared files from the connected peers");
    }
    puts(" ");
    return 1;
}

creator()
{
    puts(" ");
    puts("Full Name ==> Ashwin Mittal");
    puts("UBIT Name ==> ashwinmi");
    puts("UB Email ==> ashwinmi@buffalo.edu");
    puts(" ");
    return 1;
}

display(char *ip, char *port)
{
    puts(" ");
    char buff1[100] = "Your IP Address ==> ";
    strncat(buff1,ip,100);

    char buff2[100] = "Listening Port ==> ";
    strncat(buff2,port,100);

    puts(buff1);
    puts(buff2);
    puts(" ");
    return 1;
}

char *trimwhitespace(char *str)
{
    char *end;

    // Trim leading space
    while(isspace(*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace(*end)) end--;

    // Write new null terminator
    *(end+1) = 0;

    return str;
}

char *tokenize(char *str)
{
    char *arr[3];
    char* t;
    t = strtok(str," ");
    puts(t);
    arr[0] = t;
    t = strtok('\0'," ");
    arr[1] = t;
    t = strtok('\0'," ");
    puts(arr[1]);
    arr[2] = t;

    return arr;
}

int registerClient(char *port, char *server_ip, char *server_port)
{
    struct sockaddr_in remote_server;
    int client_socket;

    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
    {
        perror("ERROR: Creating Socket during Register");
        //exit(ERROR);
    }

    remote_server.sin_family = AF_INET;
    remote_server.sin_port = htons(atoi(server_port));
    inet_pton(AF_INET, server_ip, &(remote_server.sin_addr));
    bzero(&remote_server.sin_zero, 8);

    if((connect(client_socket, (struct sockaddr *)&remote_server, sizeof(remote_server))) == ERROR)
    {
        perror("ERROR: Register");
        //exit(ERROR);
    }
    return client_socket;
}

int connectPeer(char *port, char *server_ip, char *server_port)
{
    struct sockaddr_in remote_server;
    int client_socket;

    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
    {
        perror("ERROR: Creating Socket during Register");
        //exit(ERROR);
    }

    remote_server.sin_family = AF_INET;
    remote_server.sin_port = htons(atoi(server_port));
    inet_pton(AF_INET, server_ip, &(remote_server.sin_addr));
    bzero(&remote_server.sin_zero, 8);

    if((connect(client_socket, (struct sockaddr *)&remote_server, sizeof(remote_server))) == ERROR)
    {
        perror("ERROR: Connect peer");
        //exit(ERROR);
    }
    return client_socket;
}

list(struct connections connArr[5])
{
    printf("\n");
    printf("%-4s %-30s %-20s  %-10s\n", "id:","Hostname", "IP address", "Port No.");
    printf("\n");

    int i;
    for(i=0; i<MAX_CLIENTS; i++)
    {
        if(connArr[i].sock_fd != 0)
        {
            char buff[2];
            snprintf(buff, 2, "%d", connArr[i].id);
            printf("%-4s %-30s %-20s  %-10d\n", strcat(buff,":"), connArr[i].hostname, connArr[i].ipaddr, connArr[i].port);
        }
    }
    printf("\n");
    return 1;
}
/*
sendList()
{
    char *str[MAX_DATA];
    for(i=1; i<MAX_CLIENTS; i++)
    {
        char *str[MAX_DATA];
        char buff[2];
        snprintf(buff, 2, "%d", connArr[i].id);

        char buff2[5];
        snprintf(buff2, 5, "%d", connArr[i].port);

        strcat(buff,":~");

        strcat(str,strcat(,strcat(connArr[i].hostname,strcat(connArr[i].ipaddr,buff2))));
    }
    return 1;
}*/

set_port(struct connections connArr[5], int *sd, char *port)
{
    int j;
    for (j = 1; j < MAX_CLIENTS; j++)
    {
        if(connArr[j].sock_fd == sd)
        {
            connArr[j].port = atoi(port);
            break;
        }
    }
    return 1;
}

sendfile(int *sd, char *file)
{
    FILE *fp;
    int i, seek, init=0;
    char buffer[5000];
    fp = fopen(file, "r");

    char buff2[10] = "file|-|";
    strncat(buff2,file,20);
    char buff3[5] = "~";
    strncat(buff2,buff3,50);

    if (fp==NULL)
    {
        char *msg = "msg|-|File does not exist";
        if(send(sd, msg, strlen(msg), 0) != strlen(msg) )
        {
            perror("ERROR: Sending file");
        }
        perror ("ERROR: Opening file");
    }
    else
    {
        int num_chars = 0;
        int ch;
        int j;
        struct connections connArr[5];
        char *hostname[MAX_DATA];
        while (!feof(fp))
        {
            ch = fgetc(fp);
            buffer[num_chars] = ch;
            num_chars++;
            if(num_chars==5000)
            {
                break;
            }
        }
        buffer[num_chars+1] = '\0';
        char buff3[5000] = {0};
        snprintf(buff3, 5000, "%s", buffer);

        strncat(buff2,buff3,strlen(buff3)+strlen(buff3)+1);
        for (j = 1; j < MAX_CLIENTS; j++)
        {
            if(connArr[j].sock_fd==sd)
            {
                //puts(connArr[j].hostname);
                strcpy(hostname ,connArr[j].hostname);
                break;
            }
        }

        if(send(sd, buff2, strlen(buff2), 0) != strlen(buff2) )
        {
            perror("ERROR: Sending port");
        }
        else
        {
            //puts("File being sent");
            printf("File %s is being sent %s\n",file,hostname);
        }
    }
    fclose(fp);
    return(0);
}
