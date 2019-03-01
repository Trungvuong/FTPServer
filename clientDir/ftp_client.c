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
int sendFile(char* filename);

//Port information
int sockfd, portno, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char buffer[256];
char* command;
char* param;

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
    //printf("Please Connect to Port: ");
    strtok(connectInput, " ");
    //printf("Please Connect to Port: ");
    firstParam  = strtok(NULL, " ");
    //printf("Please Connect to Port: ");
    secondParam = strtok(NULL, " ");
    //printf("Please Connect to Port: ");


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

        command = strtok(buffer, " ");
        param = strtok(NULL, " ");

        //breaking out of while loop
        if(strcmp(command, "QUIT\n") == 0){
            printf("Closing Connection\n");
            break;
        }

        else if((strcmp(command, "RETRIEVE") == 0)){
            readServerFile();
            write(sockfd, buffer, strlen(buffer));
        }

        else if( (strcmp(command, "STORE") == 0)){
            char* realparam = param;
            realparam = strtok(realparam, "\n");
            realparam = strtok(realparam, "\0");
            //printf("real param: %s\n", realparam);
            sendFile(realparam);
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

    //printf("Inside connectPort: %s : %s \n", serverName, portNum);
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

        printf("Listing: %s\n", buffer);
        total += bytesSent;
        temp = bytesSent;
        bytesLeft -= bytesSent;
        //printf("Inwhile\n");
    }

    //printf("Exited read while\n");
    return total;
}

int readServerFile(){

    //printf("About to open file\n");
    param = strtok(param, "\n");
    FILE *fp = fopen(param, "w");
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }


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

        //write file here
        //printf("Printing to file\n");
        fprintf(fp, "%s", buffer );

        total += bytesSent;
        temp = bytesSent;
        bytesLeft -= bytesSent;
        //printf("Inwhile\n");
    }

    fclose(fp);
    //printf("Exited read while\n");
    return total;
}

int sendFile(char* filename){

    //char max[100000];
    //printf("About to open %s\n", filename);
    long length = 0;
    FILE *fp = fopen(filename, "r");

    if (fp == NULL){
        printf("Could not open file %s",filename);
        return 1;
    }
    while (fgets(buffer, 256, fp) != NULL)
        printf("%s", buffer);

    fclose(fp);

    //printf("Put file in buffer: %s\n", buffer);
    write(sockfd, buffer, strlen(buffer));
    //printf("Put file in buffer: %s\n", buffer);
    return (int)length;
}
