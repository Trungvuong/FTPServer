/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>

void error(char* msg);
char* list();
int retrieve(char* filename);
int store(char* filename);
int sendMSG();
int writeClient(char* s, int newsockfd, int fileLeng);
char target[256];

void* sockThread(void* sock);
void sigHandC(int);

//socket info
int sockfd, portno, clilen;

//variable to transfer files
char buffer[256];
char* quit = "QUIT\n";

struct sockaddr_in serv_addr, cli_addr;
int n;

int main(int argc, char *argv[])
{
    int* newsockfd;
    int status;
    pthread_t newThread;
    signal(SIGINT, sigHandC);
/*
 * TODO: Implement LIST, RETRIEVE, STORE, MULTITHREADING
 * Also fix the warnings
 */

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    //creating socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    //binding socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    //listening
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    //accepting connection
    while(1){
        newsockfd = malloc(sizeof(int));
        *newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
        if (*newsockfd < 0)
            error("ERROR on accept");

        if (status = pthread_create(&newThread, NULL, sockThread, newsockfd) != 0) {
            fprintf(stderr, "thread create error %d: %s \n", status, strerror(status));
            free(newsockfd);
            exit(1);
        }
    }


/*
    while(1) {

        char *command;
        char *param;
        char w[256];

        bzero(w, 256);
        bzero(buffer, 256);

        //reading from buffer
        //while(1) {
            n = read(newsockfd, buffer, sizeof(buffer));
            if (n == 0) {
                break;
            }
            if (n < 0)
                error("ERROR reading from socket");

        //}

        command = strtok(buffer, " ");
        param = strtok(NULL, " ");

        printf("Command: %s\n", command);

        if(strcmp(command, "QUIT\n") == 0){
            printf("Shutting Down\n");
            writeClient("Server Shutting Down\n");
            break;
        }

        else if(strcmp(command, "RETRIEVE\n") == 0){
            retrieve(param);
        }

        else if(strcmp(command, "LIST\n") == 0){
            char* l;
            l = list();
            writeClient(l);
            bzero(target, 256);
        }

        else if(strcmp(command, "STORE\n") == 0){
            store(param);
        }

        else{
            printf("Your input: %s\n", buffer);
            writeClient("Successfully Sent\n");

        }
    }
    return 0;
    */
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

char* list(){

    //char *tmp = "";
    struct dirent *de;// Pointer for directory entry


    // opendir() returns a pointer of DIR type.
    DIR *dr = opendir(".");

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory" );
        return NULL;
    }

    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    //for readdir()
    while ((de = readdir(dr)) != NULL) {
        printf("Trying to concat\n");
        printf("%s\n", de->d_name);
        strcat(target, de->d_name);
        strcat(target, " ");
    }

    closedir(dr);
    return target;
}

int retrieve(char* filename){
    return 0;
}

int store(char* filename){
    return 0;
}

int sendMSG(){

    printf("Here is the message: %s\n", buffer);
    return 0;
}

int writeClient(char* s, int newsockfd, int fileLeng){
    int ssize = strlen(s);
    char* temp = malloc(ssize + sizeof(s)*sizeof(s));
    char temp2[80];
    sprintf(temp2, "%d", ssize );
    temp = strcat(temp2, s);

    int total = 0;
    int bytesLeft = fileLeng; // how many bytes left to send
    int bytesSent = 0; //how many bytes sent

    while(total < fileLeng) {
        bytesSent = write(newsockfd, temp + total, bytesLeft);
        if (bytesSent == -1) {
            error("ERROR writing to socket");
            break;
        }

        total += bytesSent;
        bytesLeft -= bytesSent;
    }
}

void* sockThread (void* sockThread){
    while(1) {

        char *command;
        char *param;
        char w[256];

        bzero(w, 256);
        bzero(buffer, 256);

        //reading from buffer
        //while(1) {
        n = read(*(int*) sockThread, buffer, sizeof(buffer));
        if (n == 0) {
            break;
        }
        if (n < 0)
            error("ERROR reading from socket");

        //}

        command = strtok(buffer, " ");
        param = strtok(NULL, " ");

        printf("Command: %s\n", command);

        if(strcmp(command, "QUIT\n") == 0){
            printf("Shutting Down\n");
            //writeClient("Server Shutting Down\n", *(int*) sockThread);
            break;
        }

        else if(strcmp(command, "RETRIEVE\n") == 0){
            retrieve(param);
        }

        else if(strcmp(command, "LIST\n") == 0){
            char* l;
            l = list();
            writeClient(l, *(int*) sockThread, (strlen(l) + 1));
            bzero(target, 256);
        }

        else if(strcmp(command, "STORE\n") == 0){
            store(param);
        }

        else{
            printf("Your input: %s\n", buffer);
            writeClient("Successfully Sent\n", *(int*) sockThread, 256);

        }
    }
    close(*(int*) sockThread);
    free(sockThread);
    return NULL;
}

void sigHandC(int sigC) {
    printf("Shutting down server.\n");
    close(sockfd);
    exit(0);
}
