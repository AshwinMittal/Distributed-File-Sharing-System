#include "header.h"
main(int argc, char *argv[])
{
    if(argc<3)
    {
        printf("ERROR: Invalid Arguments!\n");
        printf("Arguments should be of the format => [type] [port_no] where [type] = 's' or 'c' for 'server' and 'client' respectively\n");
        exit(ERROR);
    }
    else if(atoi(argv[2])<=MIN_PORT)
    {
        printf("ERROR: Port No. should be greater than 1025!\n");
        exit(ERROR);
    }
    else if(strcmp("s",argv[1])==0 || strcmp("c",argv[1])==0)
    {
        int mode;
        if(strcmp("s",argv[1])==0)
        {
            mode = SERVER_MODE;
        }
        else
        {
            mode = CLIENT_MODE;
        }
        buildSocket(mode, argv[2]);
    }
    else
    {
        printf("ERROR: Invalid Arguments!\n");
        printf("Arguments should be of the format => [type] [port_no] where [type] = 's' or 'c' for 'server' and 'client' respectively\n");
        exit(ERROR);
    }
}
