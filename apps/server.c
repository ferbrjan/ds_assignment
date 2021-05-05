#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "lines.h"

#define MAX_LINE 256
#define TRUE 1
#define FALSE 0
int busy;
int kill;
pthread_t thread;
pthread_attr_t attr;
pthread_mutex_t mutex1;
pthread_cond_t signal1;

int register_user();
int unregister_user();
int connect_user();
int disconnect_user();
int send_req_user();
int send_mess_ack_user();
int send_mess_to_user();

//FUNCTIONS
//REGISTER
int register_user(){
    return 0;
}

//UNREGISTER
int unregister_user(){
    return 0;
}

//CONNECT
int connect_user(){
    return 0;
}

//DISCONNECT
int disconnect_user(){
    return 0;
}

//SEND_REQ
int send_req_user(){
    return 0;
}

//SEND_MESS_ACK
int send_mess_ack_user(){
    return 0;
}

//SEND_MESS
int send_mess_to_user(){
    return 0;
}

//Manage request
void manage_request (int *s) {
    int sc;
    int  res;
    kill=FALSE;
    pthread_mutex_lock(&mutex1);
    char buffer[MAX_LINE];
    int n;
    printf("thread connected as well GJ\n");
    sc=*s;
    busy=FALSE;
    pthread_cond_signal(&signal1);
    pthread_mutex_unlock(&mutex1);
    while(1){
        int msg = readLine(sc, buffer, MAX_LINE);
        if(strncmp(buffer,"REGISTER",8)==0){
            res = register_user();
            break;
        }
        else if(strncmp(buffer,"UNREGISTER",10)==0){
            res = unregister_user();
            break;
        }
        else if(strncmp(buffer,"CONNECT",7)==0){
            res=connect_user();
            break;
        }
        else if(strncmp(buffer,"DISCONNECT",10)==0){
            res=disconnect_user();
            break;
        }
        else if(strncmp(buffer,"SEND",4)==0){
            res=send_req_user();
            res=send_mess_to_user();
            break;
        }
        else if(strncmp(buffer,"SEND_MESS_ACK",13)==0){
            res=send_mess_ack_user();
            break;
        }
        else{
            break;
        }
    }
    close(sc);
    pthread_exit(NULL);
}

//MAIN
int main(int argc, char *argv[])
{
        //Variables declarations
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_mutex_init(&mutex1,NULL);
        pthread_cond_init(&signal1,NULL);
        int sd;
        int val;
        socklen_t size;
        struct sockaddr_in server_addr,client_addr;
        int err;
        int connection;
        //Create and set up socket
        sd =  socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sd < 0) {
                perror("Error in socket");
                exit(1);
        }
        val = 1;
        err = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val,
                sizeof(int));
        if (err < 0) {
                perror("Error in option");
                exit(1);
        }
        bzero((char *)&server_addr, sizeof(server_addr));
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(4200);
        connection = bind(sd,&server_addr,sizeof(server_addr));
        if (connection < 0){
        perror("Error in connecting");
        exit(1);
        }
        connection = listen(sd,5);
        if (connection < 0) {
            perror("Error in connecting");
            exit(1);
        }
        //While for accepting connections
        while(1){
            printf("waiting  for connection\n");
            int sc = accept(sd,(struct sockaddr *)&client_addr,&size);
            pthread_create(&thread,&attr,manage_request,&sc); //HERE!!!!!
            pthread_mutex_lock(&mutex1);
            while(busy==TRUE){
                pthread_cond_wait(&mutex1,&signal1);
            }
            pthread_mutex_unlock(&mutex1);
            busy=TRUE;
            if (sc < 0){
                perror("NOT CONNECTED");
                exit(1);
            }
            else{
                printf("connected\n");
            }
            if (kill==TRUE){
                exit(1);
            }
        }
        close (sd);
        return(0);
}
