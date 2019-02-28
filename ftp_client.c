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
int readServer();
int readServerFile();

//Port information
int sockfd, portno, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char buffer[256];

int main()
{

/*
 * TODO: Implement LIST, RETRIEVE, STORE, MULTITHREADING
 * Also fix the warnings
 */
    /*
     * Loop to keep entering commands
     */
    char connectInput[256];
    char* firstParam;
    char* secondParam;
    printf("Please Connect to Port: ");
    fgets(connectInput, 256, stdin);
    printf("Please Connect to Port: ");
    strtok(connectInput, " ");
    printf("Please Connect to Port: ");
    firstParam  = strtok(NULL, " ");
    printf("Please Connect to Port: ");
    secondParam = strtok(NULL, " ");
    printf("Please Connect to Port: ");


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
        if(strcmp(buffer, "QUIT\n") == 0){
            printf("Closing Connection\n");
            break;
        }

        if((strcmp(buffer, "RETRIEVE\n") == 0) || (strcmp(buffer, "STORE\n") == 0)){
            readServerFile();
        }

        else {
            readServer();
        }
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

int readServer(){
    int total = 0;
    int bytesLeft = sizeof(buffer); // how many bytes left to send
    int bytesSent = 1;
    int temp;
    int first = 1;
    int size = 1;
    //how many bytes sent

    //reading from server
    bzero(buffer, 256);
    while(total < size) {

        bytesSent = read(sockfd, buffer, sizeof(buffer));
        if (bytesSent == -1) {
            error("ERROR reading from socket");
            break;
        }
        if(first){
            size = (int)buffer[79];
            first = 0;
        }

        printf("%s\n", buffer);
        total += bytesSent;
        temp = bytesSent;
        bytesLeft -= bytesSent;
        printf("Inwhile\n");
    }

    printf("Exited read while\n");
    return total;
}

int readServerFile(){
 return 0;
}
