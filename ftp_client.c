#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

//helper functions
void error(char *msg);
int connectPort(char* sName, char* pNum);

int sockfd, portno, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char buffer[256];
char* quit = "QUIT\n";

int main()
{

/*
 * TODO: Implement LIST, RETRIEVE, STORE, MULTITHREADING
 * Also fix the warnings
 */
    /*
     * Loop to keep entering commands
     */
    char* connectInput;
    char* firstParam;
    char* secondParam;
    printf("Please Connect to Port: ");
    fgets(connectInput, 256, stdin);
    strtok(connectInput, " ");
    firstParam  = strtok(NULL, " ");
    secondParam = strtok(NULL, " ");


    if (connectPort(firstParam, secondParam) == 0){
        printf("Connected Successfully!\n");
    }


    while(1) {

        //writing to server
        printf("Please Enter A Command: ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0)
            error("ERROR writing to socket");

        //breaking out of while loop
        if(strcmp(buffer, quit) == 0){
            printf("Closing Connection\n");
            break;
        }

        //reading from server
        bzero(buffer, 256);
        //while(1) {
            n = read(sockfd, buffer, sizeof(buffer));
            //if(n == 0){
                //break;
           // }
            if (n < 0)
                error("ERROR reading from socket");
            printf("%s\n", buffer);

        //}

    }
    return 0;
}

void error(char *msg){
perror(msg);
exit(0);
}

int connectPort(char* serverName, char* portNum){

    printf("Inside connectPort: %s : %s \n", serverName, portNum);
    portno = atoi(portNum);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(serverName);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return 0;
}


