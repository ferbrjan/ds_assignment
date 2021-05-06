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

struct registered{
    char id[254];
    int port;
    struct registered* pNext;
};

struct connected{
    char id[254];
    int port;
    struct registered* pNext;
};

struct registered* pHeadReg = NULL;
struct connected* pHeadCon = NULL;

//FUNCTION DECLARATIONS
int addReg(char* id, int* port);
int searchReg(char* id);
int deleteReg(char* id);
int numReg(void);
int addCon(char* id, int* port);
int searchCon(char* id);
int deleteCon(char* id);
int numCon(void);
int register_user(char* user);
int unregister_user(char* user);
int connect_user(char* user, int port);
int disconnect_user(char* user);
int send_req_user();
int send_mess_ack_user();
int send_mess_to_user();

//FUNCTIONS
//REGISTER
int register_user(char* user){
    printf("In register_user()!\n");
    int res = searchReg(user);
    if (res==1){
        printf("user already exists\n");
        return  1;
    }
    else{
        addReg(user, 4200);
        printf("user registered\n");
        printf("number of  registered users is %i\n",numReg());
        return 0;
    }
}

//UNREGISTER
int unregister_user(char* user){
    printf("In unregister_user()!\n");
    int res = searchReg(user);
    if (res==0){
        printf("user does not exists\n");
        return  1;
    }
    else{
        deleteReg(user);
        printf("user unregistered\n");
        printf("number of  registered users is %i\n",numReg());
        return 0;
    }
}

//CONNECT
int connect_user(char* user, int port){
    printf("In connect_user()!\n");
    int res = searchReg(user);
    if (res==0){
        printf("user not registered\n");
        return  1;
    }
    else{
        int res = searchCon(user);
        if (res==1){
            printf("user already connected\n");
            return  2;
        }
        else{
            addCon(user,port);
            printf("user connected\n");
            printf("number of  connected users is %i\n",numCon());
            return 0;
        }
    }
}

//DISCONNECT
int disconnect_user(char* user){
    printf("In disconnect_user()!\n");
    int res = searchReg(user);
    if (res==0){
        printf("user not registered\n");
        return  1;
    }
    else{
        int res = searchCon(user);
        if (res==1){
            deleteCon(user);
            printf("user disconnecting\n");
            printf("number of  connected users is %i\n",numCon());
            return  0;
        }
        else{
            printf("user not connected\n");
            return 2;
        }
    }
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
            msg = readLine(sc, buffer, MAX_LINE);
            res = register_user(buffer);
            break;
        }
        else if(strncmp(buffer,"UNREGISTER",10)==0){
            msg = readLine(sc, buffer, MAX_LINE);
            res = unregister_user(buffer);
            break;
        }
        else if(strncmp(buffer,"CONNECT",7)==0){
            msg = readLine(sc, buffer, MAX_LINE);
            char buffer2[MAX_LINE];
            msg = readLine(sc, buffer2, MAX_LINE);
            int port = atoi(buffer2);
            res=connect_user(buffer,port);
            break;
        }
        else if(strncmp(buffer,"DISCONNECT",10)==0){
            msg = readLine(sc, buffer, MAX_LINE);
            res=disconnect_user(buffer);
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
int addReg(char* id, int* port)
{
    struct registered* new = (struct registered*)malloc(sizeof(struct registered));
    strcpy(new->id,id);
    new->port = port;
    new->pNext = pHeadReg;
    pHeadReg = new;
    return 0;
}

int searchReg(char* id)
{
    struct registered* tmp = pHeadReg;
    while(tmp != NULL)
    {
        if(strcmp(id, tmp->id) == 0)
            return 1;
        tmp = tmp->pNext;
    }
    return 0;//element does not exsist
}


int deleteReg(char* id)
{
    struct registered* prev = NULL;
    struct registered* tmp = pHeadReg;
    while(tmp)
    {
        if(!strcmp(id, tmp->id))
        {
            if(prev!=NULL)
                prev->pNext = tmp->pNext;
            else
                pHeadReg = tmp->pNext;
            free(tmp);
            return 0;
        }
        prev = tmp;
        tmp = tmp->pNext;
    }
    return -1;//element does not exsist
}

int numReg()
{
    int num = 0;
    struct registered* tmp = pHeadReg;
    while(tmp)
    {
        num = num + 1;
        tmp = tmp->pNext;
    }
    return num;
}

int addCon(char* id, int* port)
{
    struct connected* new = (struct connected*)malloc(sizeof(struct connected));
    strcpy(new->id,id);
    new->port = port;
    new->pNext = pHeadCon;
    pHeadCon = new;
    return 0;
}

int searchCon(char* id)
{
    struct connected* tmp = pHeadCon;
    while(tmp != NULL)
    {
        if(strcmp(id, tmp->id) == 0)
            return 1;
        tmp = tmp->pNext;
    }
    return 0;//element does not exsist
}


int deleteCon(char* id)
{
    struct connected* prev = NULL;
    struct connected* tmp = pHeadCon;
    while(tmp)
    {
        if(!strcmp(id, tmp->id))
        {
            if(prev!=NULL)
                prev->pNext = tmp->pNext;
            else
                pHeadCon = tmp->pNext;
            free(tmp);
            return 0;
        }
        prev = tmp;
        tmp = tmp->pNext;
    }
    return -1;//element does not exsist
}

int numCon()
{
    int num = 0;
    struct connected* tmp = pHeadCon;
    while(tmp)
    {
        num = num + 1;
        tmp = tmp->pNext;
    }
    return num;
}
