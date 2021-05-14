#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "../include/lines.h"

#define MAX_LINE 256
#define TRUE 1
#define FALSE 0
int busy;
int kill;
pthread_t thread;
pthread_attr_t attr;
pthread_mutex_t mutex1;
pthread_cond_t signal1;



struct messages{
    char msg[256];
    char from[254];
    char to[254]; 
    int ACK;
    unsigned int msg_id;
    struct messages* pNext;
};



struct registered{
    char id[254];
    unsigned int msg_id;
    int port;
    struct registered* pNext;
};

struct connected{
    char id[254];
    int port;
    char ip[254];
    struct connected* pNext;
};

struct registered* pHeadReg = NULL;
struct connected* pHeadCon = NULL;
struct messages* pHeadMsg = NULL;

//FUNCTION DECLARATIONS
int searchConIpPort(char* p_id, char * p_ip, int p_port);
int addReg(char* id, int port);
int addMsg(char* p_msg, char* p_from, char* p_to);
int searchMsg(char *p_from,unsigned int * p_msg_id, char* p_msg, char* p_to);
int searchReg(char* id);
int deleteReg(char* id);
int numReg(void);
int addCon(char* id,int port,char* ip);
int searchCon(char* id);
int getConIpPort(char* p_id, char* p_ip, char * p_port);
int deleteCon(char* id);
int numCon(void);
int register_user(char* user, int sc);
int unregister_user(char* user, int sc);
int connect_user(char* user, int port, int sc,char* ip);
int disconnect_user(char* user, int sc);
int send_req_user(char* user, char* message,char* port, int sc);
int send_mess_ack_user();
int send_mess_to_user();

//FUNCTIONS
//REGISTER
int register_user(char* user, int sc){
    char buffer[MAX_LINE];
    int msg;
    printf("In register_user()!\n");
    int res = searchReg(user);
    if (res==1){
        printf("user already exists\n");
        strcpy(buffer,"1");
        msg=sendMessage(sc, buffer, strlen(buffer));
        //strcpy(buffer,"USERNAME IN USE");
        //msg=sendMessage(sc, buffer, strlen(buffer));
        return  1;
    }
    if (res==0){
        addReg(user, 4200);
        printf("user registered\n");
        printf("number of  registered users is %i\n",numReg());
        strcpy(buffer,"0");
        msg=sendMessage(sc, buffer, strlen(buffer));
        //strcpy(buffer,"REGISTER OK");
        //msg=sendMessage(sc, buffer, strlen(buffer));
        return 0;
    }
    else{
        strcpy(buffer,"2");
        msg=sendMessage(sc, buffer, strlen(buffer));
        //strcpy(buffer,"REGISTER FAIL");
        //msg=sendMessage(sc, buffer, strlen(buffer));
        return 2;
    }
}

//UNREGISTER
int unregister_user(char* user, int sc){
    char buffer[MAX_LINE];
    int msg;
    printf("In unregister_user()!\n");
    int res = searchReg(user);
    if (res==0){
        strcpy(buffer,"1");
        msg=sendMessage(sc, buffer, strlen(buffer));
        printf("user does not exists\n");
        //strcpy(buffer,"USER DOES NOT EXIST");
        //msg=sendMessage(sc, buffer, strlen(buffer));
        return 1;
    }
    if (res==1){
        res = searchCon(user);
        if (res==1){
            strcpy(buffer,"2");
            msg=sendMessage(sc, buffer, strlen(buffer));
            printf("user needs to disconnect first\n");
            //strcpy(buffer,"UNREGISTER FAIL");
            //msg=sendMessage(sc, buffer, strlen(buffer));
            return 2;
        }
        else{
            deleteReg(user);
            printf("user unregistered\n");
            printf("number of  registered users is %i\n",numReg());
            strcpy(buffer,"0");
            msg=sendMessage(sc, buffer, strlen(buffer));
            //strcpy(buffer,"UNREGISTER OK");
            //msg=sendMessage(sc, buffer, strlen(buffer));
            return 0;
        }
    }
    else{
        strcpy(buffer,"2");
        msg=sendMessage(sc, buffer, strlen(buffer));
        //strcpy(buffer,"UNREGISTER FAIL");
        //msg=sendMessage(sc, buffer, strlen(buffer));
        return 2;
    }
}

//CONNECT
int connect_user(char* user, int port, int sc,char* ip){
    char buffer[MAX_LINE];
    int msg;
    printf("In connect_user()!\n");
    printf("User's ip is:%s\n",ip);
    int res = searchReg(user);
    if (res==0){
        printf("user not registered\n");
        strcpy(buffer,"1");
        msg=sendMessage(sc, buffer, strlen(buffer));
        //strcpy(buffer,"CONNECT FAIL, USER DOES NOT EXIST");
        //msg=sendMessage(sc, buffer, strlen(buffer));
        return  1;
    }
    else{
        int res = searchCon(user);
        if (res==1){
            printf("user already connected\n");
            strcpy(buffer,"2");
            msg=sendMessage(sc, buffer, strlen(buffer));
            //strcpy(buffer,"USER ALREADY CONNECTED");
            //msg=sendMessage(sc, buffer, strlen(buffer));
            return  2;
        }
        if (res==0){
            addCon(user,port,ip);
            printf("user connected\n");
            printf("number of  connected users is %i\n",numCon());
            strcpy(buffer,"0");
            msg=sendMessage(sc, buffer, strlen(buffer));
            //strcpy(buffer,"CONNECT OK");
            //msg=sendMessage(sc, buffer, strlen(buffer));
            return 0;
        }
        else{
            strcpy(buffer,"3");
            msg=sendMessage(sc, buffer, strlen(buffer));
            //strcpy(buffer,"CONNECT FAIL");
            //msg=sendMessage(sc, buffer, strlen(buffer));
            return 2;
        }
    }
}

//DISCONNECT
int disconnect_user(char* user, int sc){
    char buffer[MAX_LINE];
    int msg;
    printf("In disconnect_user()!\n");
    int res = searchReg(user);
    if (res==0){
        printf("user not registered\n");
        strcpy(buffer,"1");
        msg=sendMessage(sc, buffer, strlen(buffer));
        //strcpy(buffer,"DISCONNECT FAIL/ USER DOES NOT EXIST");
        //msg=sendMessage(sc, buffer, strlen(buffer));
        return  1;
    }
    else if (res==1){
        int res = searchCon(user);
        if (res==1){
            deleteCon(user);
            printf("user disconnecting\n");
            printf("number of  connected users is %i\n",numCon());
            strcpy(buffer,"0");
            msg=sendMessage(sc, buffer, strlen(buffer));
            //strcpy(buffer,"DISCONNECT OK");
            //msg=sendMessage(sc, buffer, strlen(buffer));
            return  0;
        }
        else{
            printf("user not connected\n");
            strcpy(buffer,"2");
            msg=sendMessage(sc, buffer, strlen(buffer));
            //strcpy(buffer,"DISCONNECT FAIL/ USER NOT CONNECTED");
            //msg=sendMessage(sc, buffer, strlen(buffer));
            return 2;
        }
    }
    else{
        strcpy(buffer,"3");
        msg=sendMessage(sc, buffer, strlen(buffer));
        //strcpy(buffer,"DISCONNECT FAIL");
        //msg=sendMessage(sc, buffer, strlen(buffer));
        return 3;
    }
}

//SEND_REQ
int send_req_user(char* user, char* message,char* port, int sc){ //user = reciever , message = message to be sent, port = port of the connected user in order to search for his username in combination with IP adress in  case there are two users on the same computer
    char buffer[MAX_LINE];
    int msg;
    char sender[254];
    char user_ip[254];
    char user_port[254];
    int user_port_number;
    int port_number=atoi(port);
    
    printf("MESSAGE SENT IS: %s\n",message);
    
    struct sockaddr_in client_server_addr;
    socklen_t serv_len = sizeof(client_server_addr);
    getpeername(sc,(struct sockaddr *)&client_server_addr,&serv_len);
    
    printf("function returns %i\n",searchConIpPort((char*)sender,inet_ntoa(client_server_addr.sin_addr),port_number));
    
    printf("NAME OF THE SENDER IS:%s\n",sender);
    
    int res = searchReg(user);
    //printf("AM I HERE? %i \n",res);
    if (res==1){
        printf("USER REGISTERED\n");
        res = searchCon(user);
        if (res==1){
            printf("USER CONNECTED\n");
            strcpy(buffer,"0");
            msg=sendMessage(sc, buffer, strlen(buffer));
            getConIpPort(user,user_ip,user_port);
            user_port_number=atoi(user_port);
            //Connection to the socket
            int sock = 0, valread;
            if ((sock = socket(AF_INET,SOCK_STREAM, 0)) < 0)
            {
                printf("\n Socket creation error \n");
                //return -1;
            }
            client_server_addr.sin_family = AF_INET;
            client_server_addr.sin_port = htons(user_port_number); //port number of reciever
            // Convert IPv4 and IPv6 addresses from text to binary form
            if(inet_pton(AF_INET, user_ip , &client_server_addr.sin_addr)<=0) //IP of  reciever
            {
                printf("\nInvalid address/ Address not supported \n");
                //return -1;
            }
            if (connect(sock, (struct sockaddr *)&client_server_addr, sizeof(client_server_addr)) < 0)
            {
                printf("\nConnection Failed \n");
                //return -1;
            }
            //printf("I am sending to IP: %s PORT: %s\n",user_ip,user_port);
            addMsg(message,sender,user);
            unsigned int id;
            int i = searchMsg(sender,&id, message, user);
            char str[254];
            sprintf(str, "%d ", id);
            msg=sendMessage(sock, str, strlen(str));
            strcat(sender, " ");
            msg=sendMessage(sock, sender, strlen(sender));
            strcat(message, "\n");
            msg=sendMessage(sock, message, strlen(message));
            close(sock);
            printf("message was sent\n");
            return 0;
        }
        else{
            printf("USER NOT CONNECTED\n");
            addMsg(message,sender,user);
            strcpy(buffer,"0");
            msg=sendMessage(sc, buffer, strlen(buffer));
            return 0;
        }
    }
    else if (res==0){
        printf("USER DOES NOT EXIST\n");
        strcpy(buffer,"1");
        msg=sendMessage(sc, buffer, strlen(buffer));
        return 1;
    }
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
    int sc,client_sc;
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
            res = register_user(buffer,sc);
            close(sc);
            break;
        }
        else if(strncmp(buffer,"UNREGISTER",10)==0){
            msg = readLine(sc, buffer, MAX_LINE);
            res = unregister_user(buffer,sc);
            close(sc);
            break;
        }
        else if(strncmp(buffer,"CONNECT",7)==0){
            char user_name[254];;
            msg = readLine(sc, buffer, MAX_LINE);
            strcpy(user_name, buffer);
            char buffer2[MAX_LINE];
            msg = readLine(sc, buffer2, MAX_LINE);
            int port = atoi(buffer2);
            //use getpeername to extract the  ip from the socket and then connect with the particular user
            struct sockaddr_in client_server_addr;
            socklen_t serv_len = sizeof(client_server_addr);
            getpeername(sc,(struct sockaddr *)&client_server_addr,&serv_len);
            printf("Server socket's peer ip : %s\n", inet_ntoa(client_server_addr.sin_addr));
            res=connect_user(buffer,port,sc,inet_ntoa(client_server_addr.sin_addr));
            close(sc);
            printf("The port number is:%d\n", port);
            printf("The username is:%s\n",user_name);
            int sock = 0, valread;
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                printf("\n Socket creation error \n");
                //return -1;
            }
            client_server_addr.sin_family = AF_INET;
            client_server_addr.sin_port = htons(port);
            // Convert IPv4 and IPv6 addresses from text to binary form
            if(inet_pton(AF_INET, inet_ntoa(client_server_addr.sin_addr), &client_server_addr.sin_addr)<=0) 
            {
                printf("\nInvalid address/ Address not supported \n");
                //return -1;
            }
            int i;
            char message[256];
            char from_user[254];
            unsigned int id;
            i = searchMsg(from_user,&id, message, user_name);
            i = searchMsg(from_user,&id, message, user_name);
            
            if (i>0){
                if (connect(sock, (struct sockaddr *)&client_server_addr, sizeof(client_server_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    //return -1;
                }
                while(1)//loop to search for all the messages for the connected user
                {
                    //printf("im in the while(1) loop that i will be sending the messages back\n");
                    printf("did we find any messages?\n return from function:%d\nmessage:%s\nfrom_user:%s\n",i,message,from_user);
                    printf("the number of the id of this message in while(1 )loop is %d\n", id);
                    if(i == 0)
                    {
                        close(sock);
                        break;
                    }
                    char str[254];
                    sprintf(str, "%d ", id);
                    msg=sendMessage(sock, str, strlen(str));
                    strcat(from_user, " ");
                    msg=sendMessage(sock, from_user, strlen(from_user));
                    strcat(message, "\n");
                    msg=sendMessage(sock, message, strlen(message));

                    i = searchMsg(from_user,&id, message, user_name);//IS THIS OK?? o.O
                    i = searchMsg(from_user,&id, message, user_name);
                    //int searchMsg(char *p_from,unsigned int * p_msg_id, char* p_msg, char* p_to);
                }

            }
            break;
        }
        else if(strncmp(buffer,"DISCONNECT",10)==0){
            msg = readLine(sc, buffer, MAX_LINE);
            res=disconnect_user(buffer,sc);
            if (res==0){
                close(client_sc);
            }
            close(sc);
            break;
        }
        else if(strncmp(buffer,"SEND",4)==0){
            char buffer2[MAX_LINE],port[MAX_LINE];
            msg = readLine(sc, buffer, MAX_LINE);
            printf("The reciver is:%s\n", buffer);
            msg = readLine(sc, buffer2, MAX_LINE);
            printf("The message is:%s\n", buffer2);
            msg = readLine(sc, port, MAX_LINE);
            printf("The port is:%s\n", port);
            res=send_req_user(buffer,buffer2,port,sc);
            close(sc);
            printf("CONNECTION CLOSED\n");
            break;
        }
        else if(strncmp(buffer,"SEND_MESS_ACK",13)==0){
            res=send_mess_ack_user();
            close(sc);
            break;
        }
        else{
            break;
        }
    }
    printf("THREAD EXITED\n");
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
        connection = bind(sd,(const struct sockaddr *) &server_addr,sizeof(server_addr));
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
            //printf("what is in sc %d\n", sc);
            //char *ip = inet_ntoa(client_addr.sin_addr);
            //printf("let me see the ip address in main:%s\n", ip);
            pthread_create(&thread,&attr,manage_request,&sc); //HERE!!!!!
            pthread_mutex_lock(&mutex1);
            while(busy==TRUE){
                pthread_cond_wait(&signal1,&mutex1);
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
int addReg(char* id, int port)
{
    struct registered* new = (struct registered*)malloc(sizeof(struct registered));
    strcpy(new->id,id);
    new->port = port;
    new->pNext = pHeadReg;
    new->msg_id = 0;
    pHeadReg = new;
    return 0;
}

int searchReg(char* id)
{
    struct registered* tmp = pHeadReg;
    while(tmp != NULL)
    {
        //printf("AM I IN SEARCH REG?\n");
        if(strcmp(id, tmp->id) == 0){
            //printf("AM I STILL IN SEARCH REG?\n");
            return 1;
        }
        tmp = tmp->pNext;
    }
    return 0;//element does not exsist
}
unsigned int getMsgId(char* id)
{
    struct registered* tmp = pHeadReg;
    while(tmp != NULL)
    {
        if(strcmp(id, tmp->id) == 0){
            tmp->msg_id += 1;
            return tmp->msg_id;
        }
        tmp = tmp->pNext;
    }
    return -1;//element does not exsist
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

int addCon(char* id, int port,char* ip)
{
    struct connected* new = (struct connected*)malloc(sizeof(struct connected));
    strcpy(new->id,id);
    new->port = port;
    strcpy(new->ip,ip);
    printf("port number for user: %s is %i\n",new->id,new->port);
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
int getConIpPort(char* p_id, char* p_ip, char * p_port)
{
    struct connected* tmp = pHeadCon;
    while(tmp != NULL)
    {
        if(strcmp(p_id, tmp->id) == 0)
        {
            strcpy(p_ip, tmp->ip);
            sprintf(p_port,"%d",tmp->port);
            //strcpy(p_port, tmp->port);
            return 1;
        }
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
int addMsg(char* p_msg, char* p_from, char* p_to)
{
    struct messages* new = (struct messages*)malloc(sizeof(struct messages));
    strcpy(new->msg,p_msg);
    strcpy(new->from,p_from);
    strcpy(new->to,p_to);
    new->msg_id = getMsgId(p_from);
    printf("the last message addes for user: %s has ID:%d\n", new->from, new->msg_id);
    new->ACK = 0;
    new->pNext = pHeadMsg;
    pHeadMsg = new;
    return 0;
}
int searchMsg(char *p_from,unsigned int * p_msg_id, char* p_msg, char* p_to)//search and delete
{
    struct messages* prev = NULL;
    struct messages* tmp = pHeadMsg;
    while(tmp != NULL)
    {
        if(strcmp(p_to, tmp->to) == 0)
        {
            strcpy(p_msg, tmp->msg);
            strcpy(p_from, tmp->from);
            *p_msg_id = tmp->msg_id;
            printf("the id number of message is:%d\n", *p_msg_id);
            if(tmp->ACK == 1)
            {
            //now we need to delete this message from the message list
                if(prev!=NULL)
                    prev->pNext = tmp->pNext;
                else
                    pHeadMsg = tmp->pNext;
                free(tmp);
                return 2;//send only the ACK
            }
            else
            {
                tmp->ACK = 1;
                return 1;//send the message 
            }
        }
        prev = tmp;
        tmp = tmp->pNext;
    }
    return 0;//element does not exsist
}
int searchConIpPort(char* p_id, char * p_ip, int p_port)
{
    struct connected* tmp = pHeadCon;
    while(tmp != NULL)
    {
        if((strcmp(p_ip, tmp->ip) == 0) && (p_port == tmp->port)){
            strcpy(p_id, tmp->id);
            return 1;
        }
        tmp = tmp->pNext;//it is a match
    }
    return 0;//element does not exsist
}
