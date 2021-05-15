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
int server_port;
pthread_t thread;
pthread_attr_t attr;
pthread_mutex_t mutex1;
pthread_cond_t signal1;


//Mesaage storing linked list
struct messages{
    char msg[256];
    char from[254];
    char to[254]; 
    unsigned int msg_id;
    struct messages* pNext;
};


//Registered users
struct registered{
    char id[254];
    unsigned int msg_id;
    int port;
    struct registered* pNext;
};

//Connected users
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
int searchConIpPort(char* p_id, char * p_ip, int p_port); //Serach for user based on its IP and port
unsigned int getMsgId(char* id); //Get msg id
int addReg(char* id, int port); //Add registered user
int addMsg(char* p_msg, char* p_from, char* p_to, unsigned int p_msg_id); //Add message  to list
int searchMsg(char *p_from,unsigned int * p_msg_id, char* p_msg, char* p_to); //search for message by username
int searchReg(char* id); //search for registred user
int deleteReg(char* id); //delete registred user
int numReg(void); //number of registred users
int addCon(char* id,int port,char* ip); //Add connected user
int searchCon(char* id); //Search connected user
int getConIpPort(char* p_id, char* p_ip, char * p_port); //Get ip and  port of a connected user
int deleteCon(char* id); //delete connected user
int numCon(void); //Number of connected users
//Functions used for UN/register, DIS/connect and send_mess
int register_user(char* user, int sc);
int unregister_user(char* user, int sc);
int connect_user(char* user, int port, int sc,char* ip);
int disconnect_user(char* user, int sc);
int send_req_user(char* user, char* message,char* port, int sc);

//FUNCTIONS
//REGISTER
int register_user(char* user, int sc){
    char buffer[MAX_LINE];
    int msg;
    int res = searchReg(user);
    if (res==1){
        strcpy(buffer,"1");
        msg=sendMessage(sc, buffer, strlen(buffer));
        printf("s> REGISTER %s FAIL\n",user);
        return  1;
    }
    if (res==0){
        addReg(user, server_port);
        strcpy(buffer,"0");
        msg=sendMessage(sc, buffer, strlen(buffer));
        printf("s> REGISTER %s OK\n",user);
        return 0;
    }
    else{
        strcpy(buffer,"2");
        msg=sendMessage(sc, buffer, strlen(buffer));
        printf("s> REGISTER %s FAIL\n",user);
        return 2;
    }
}

//UNREGISTER
int unregister_user(char* user, int sc){
    char buffer[MAX_LINE];
    int msg;
    int res = searchReg(user);
    //User not registered
    if (res==0){
        strcpy(buffer,"1");
        msg=sendMessage(sc, buffer, strlen(buffer));
        printf("s> UNREGISTER %s FAIL\n",user);
        return 1;
    }
    //User registered
    if (res==1){
        res = searchCon(user);
        //User connected / needs to disconnect first
        if (res==1){
            strcpy(buffer,"2");
            msg=sendMessage(sc, buffer, strlen(buffer));
            printf("s> UNREGISTER %s FAIL\n",user);
            return 2;
        }
        //User not connected all good!
        else{
            deleteReg(user);
            strcpy(buffer,"0");
            msg=sendMessage(sc, buffer, strlen(buffer));
            printf("s> UNREGISTER %s OK\n",user);
            return 0;
        }
    }
    else{
        strcpy(buffer,"2");
        msg=sendMessage(sc, buffer, strlen(buffer));
        printf("s> UNREGISTER %s FAIL\n",user);
        return 2;
    }
}

//CONNECT
int connect_user(char* user, int port, int sc,char* ip){
    char buffer[MAX_LINE];
    int msg;
    int res = searchReg(user);
    //User not registered
    if (res==0){
        strcpy(buffer,"1");
        msg=sendMessage(sc, buffer, strlen(buffer));
        printf("s> CONNECT %s FAIL\n",user);
        return  1;
    }
    //User registered
    else{
        int res = searchCon(user);
        //User connected
        if (res==1){
            strcpy(buffer,"2");
            msg=sendMessage(sc, buffer, strlen(buffer));
            printf("s> CONNECT %s FAIL\n",user);
            return  2;
        }
        //User not connected
        if (res==0){
            addCon(user,port,ip);
            strcpy(buffer,"0");
            msg=sendMessage(sc, buffer, strlen(buffer));
            printf("s> CONNECT %s OK\n",user);
            return 0;
        }
        //Weird stuff happenin (should never get here hopefuly)
        else{
            strcpy(buffer,"3");
            msg=sendMessage(sc, buffer, strlen(buffer));
            printf("s> CONNECT %s FAIL\n",user);
            return 3;
        }
    }
}

//DISCONNECT
int disconnect_user(char* user, int sc){
    char buffer[MAX_LINE];
    int msg;
    int res = searchReg(user);
    //User not registered
    if (res==0){
        strcpy(buffer,"1");
        msg=sendMessage(sc, buffer, strlen(buffer));
        printf("s> DISCONNECT %s FAIL\n",user);
        return  1;
    }
    //User registered
    else if (res==1){
        int res = searchCon(user);
        //User connected
        if (res==1){
            deleteCon(user);
            strcpy(buffer,"0");
            msg=sendMessage(sc, buffer, strlen(buffer));
            printf("s> DISCONNECT %s OK\n",user);
            return  0;
        }
        //User not connected
        else{
            strcpy(buffer,"2");
            msg=sendMessage(sc, buffer, strlen(buffer));
            printf("s> DISCONNECT %s FAIL\n",user);
            return 2;
        }
    }
    //Weird stuff happenin (should never get here hopefuly)
    else{
        strcpy(buffer,"3");
        msg=sendMessage(sc, buffer, strlen(buffer));
        printf("s> DISCONNECT %s FAIL\n",user);
        return 3;
    }
}

//SEND_REQ
int send_req_user(char* user, char* message,char* port, int sc){
    //user = reciever , message = message to be sent, port = port of the connected user in order to search for his username in combination with IP adress in  case there are two users on the same computer
    char buffer[MAX_LINE];
    int msg;
    char sender[254];
    char user_ip[254];
    char user_port[254];
    int user_port_number;
    int port_number=atoi(port);
    
    
    struct sockaddr_in client_server_addr;
    socklen_t serv_len = sizeof(client_server_addr);
    getpeername(sc,(struct sockaddr *)&client_server_addr,&serv_len);
    //distinguish if I am sending to connected or registered only
    
    int res = searchConIpPort((char*)sender,inet_ntoa(client_server_addr.sin_addr),port_number);
    
    if (res==0){
        //printf("CONNECTED user not found\n");
        strcpy(buffer,"2");
        msg=sendMessage(sc, buffer, strlen(buffer));
        getConIpPort(user,user_ip,user_port);
        user_port_number=atoi(user_port);
        printf("s> SEND FAIL\n");
        return 2;
    }
    
    res = searchReg(user);
    if (res==1){
        res = searchCon(user);
        //User connected
        if (res==1){
            strcpy(buffer,"0");
            msg=sendMessage(sc, buffer, strlen(buffer));
            getConIpPort(user,user_ip,user_port);
            user_port_number=atoi(user_port);
            
            //Connection to the socket of reciever
            int sock = 0, valread;
            if ((sock = socket(AF_INET,SOCK_STREAM, 0)) < 0)
            {
                printf("\n Socket creation error \n");
                return -1;
            }
            client_server_addr.sin_family = AF_INET;
            client_server_addr.sin_port = htons(user_port_number); //port number of reciever
            // Convert IPv4 and IPv6 addresses from text to binary form
            if(inet_pton(AF_INET, user_ip , &client_server_addr.sin_addr)<=0) //IP of  reciever
            {
                printf("\nInvalid address/ Address not supported \n");
                return -1;
            }
            if (connect(sock, (struct sockaddr *)&client_server_addr, sizeof(client_server_addr)) < 0)
            {
                printf("\nConnection Failed \n");
                return -1;
            }
            unsigned int tmp_id = getMsgId(sender);
            addMsg(message,sender,user, tmp_id);
            unsigned int id;
            int i = searchMsg(sender,&id, message, user);
            printf("s> SEND MESSAGE %d FROM %s TO %s\n",id,sender,user);
            char str[254];
            sprintf(str, "%d ", id);
            msg=sendMessage(sock, str, strlen(str));
            strcat(sender, " ");
            msg=sendMessage(sock, sender, strlen(sender));
            strcat(message, "\n");
            msg=sendMessage(sock, message, strlen(message));
            close(sock);

            //OPEN CONNECTION FOR SENDER TO SEND ACK
            getpeername(sc,(struct sockaddr *)&client_server_addr,&serv_len);
            searchConIpPort((char*)sender,inet_ntoa(client_server_addr.sin_addr),port_number);
            getConIpPort(sender,user_ip,user_port);
            sock=0;
            user_port_number=atoi(user_port);
            //Connection to the socket
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
            
            sprintf(str, "%d ", id);
            msg=sendMessage(sock, str, strlen(str));
            msg=sendMessage(sock, "ACK ", 5);
            msg=sendMessage(sock, "\n", 2);//4/5?
            close(sock);//close connection with the sender

            return 0;
        }
        //User registered
        else{
            unsigned int tmp_id = getMsgId(sender);
            addMsg(message,sender,user, tmp_id);
            strcpy(buffer,"0");
            msg=sendMessage(sc, buffer, strlen(buffer));
            printf("s> SEND MESSAGE %d FROM %s TO %s STORED\n",tmp_id,sender,user);
            return 0;
        }
    }
    //User not registered
    else if (res==0){
        strcpy(buffer,"1");
        msg=sendMessage(sc, buffer, strlen(buffer));
        printf("s> SEND FAIL\n");
        return 1;
    }
    //Weird stuff happenin (should never get here hopefuly)
    else{
        strcpy(buffer,"2");
        msg=sendMessage(sc, buffer, strlen(buffer));
        printf("s> SEND FAIL\n");
        return 2;
    }
    return 0;
}

//Manage request
void manage_request (int *s) {
    int sc,client_sc;
    int  res;
    pthread_mutex_lock(&mutex1);
    char buffer[MAX_LINE];
    int n;
    sc=*s;
    busy=FALSE;
    pthread_cond_signal(&signal1);
    pthread_mutex_unlock(&mutex1);
    
    while(1){
        int msg = readLine(sc, buffer, MAX_LINE);
        //Register
        if(strncmp(buffer,"REGISTER",8)==0){
            msg = readLine(sc, buffer, MAX_LINE);
            res = register_user(buffer,sc);
            close(sc);
            break;
        }
        //Unregister
        else if(strncmp(buffer,"UNREGISTER",10)==0){
            msg = readLine(sc, buffer, MAX_LINE);
            res = unregister_user(buffer,sc);
            close(sc);
            break;
        }
        //Connect + send pending messages and ACKs
        else if(strncmp(buffer,"CONNECT",7)==0){
            char user_name[254];;
            msg = readLine(sc, buffer, MAX_LINE);
            strcpy(user_name, buffer);
            char buffer2[MAX_LINE];
            msg = readLine(sc, buffer2, MAX_LINE);
            int port = atoi(buffer2);
            //use getpeername to extract the ip from the socket and then connect with the particular user
            struct sockaddr_in client_server_addr;
            socklen_t serv_len = sizeof(client_server_addr);
            getpeername(sc,(struct sockaddr *)&client_server_addr,&serv_len);
            res=connect_user(buffer,port,sc,inet_ntoa(client_server_addr.sin_addr));
            close(sc);
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
            
            
            if (i>0){
                if (connect(sock, (struct sockaddr *)&client_server_addr, sizeof(client_server_addr)) < 0)
                {
                    printf("\nConnection Failed \n");
                    //return -1;
                }
                while(1)//loop to search for all the messages for the connected user
                {
                    if(i == 0)
                    {
                        close(sock);
                        break;
                    }
                    //Message is not an ACK (send message  -> send ack)
                    if(strcmp(from_user,"ACK")!=0)
                    {
                        printf("s> SEND MESSAGE %d FROM %s TO %s\n",id,from_user,user_name);
                        char str[254];
                        sprintf(str, "%d ", id);
                        msg=sendMessage(sock, str, strlen(str));
                        char reciever [254];
                        strcpy(reciever,from_user);
                        strcat(from_user, " ");
                        msg=sendMessage(sock, from_user, strlen(from_user));
                        strcat(message, "\n");
                        msg=sendMessage(sock, message, strlen(message));
                        res = searchCon(reciever);
                        if (res==1){
                            struct sockaddr_in sclient_server_addr;
                            socklen_t sserv_len = sizeof(sclient_server_addr);
                            char user_ip[254];
                            char user_port[254];
                            int user_port_number;
                            getConIpPort(reciever,user_ip,user_port);
                            int soc=0;
                            user_port_number=atoi(user_port);
                            
                            //Connection to the socket of sender in order to send an ACK if he connected
                            if ((soc = socket(AF_INET,SOCK_STREAM, 0)) < 0)
                            {
                                printf("\n Socket creation error \n");
                                //return -1;
                            }
                            sclient_server_addr.sin_family = AF_INET;
                            sclient_server_addr.sin_port = htons(user_port_number); //port number of reciever
                            // Convert IPv4 and IPv6 addresses from text to binary form
                            if(inet_pton(AF_INET, user_ip , &sclient_server_addr.sin_addr)<=0) //IP of  reciever
                            {
                                printf("\nInvalid address/ Address not supported \n");
                                //return -1;
                            }
                            if (connect(soc, (struct sockaddr *)&sclient_server_addr, sizeof(sclient_server_addr)) < 0)
                            {
                                printf("\nConnection Failed \n");
                                //return -1;
                            }
                            sprintf(str, "%d ", id);
                            msg=sendMessage(soc, str, strlen(str));
                            msg=sendMessage(soc, "ACK ", 5);
                            msg=sendMessage(soc, "\n", 2);
                            close(soc);
                        }
                        else{
                            addMsg(" ", "ACK", reciever, id);
                        }
                    }
                    //Message is an ACK (send ack only)
                    else{
                        char str[254];
                        sprintf(str, "%d ", id);
                        msg=sendMessage(sock, str, strlen(str));
                        strcat(from_user, " ");
                        msg=sendMessage(sock, from_user, strlen(from_user));
                        strcat(message, "\n");
                        msg=sendMessage(sock, message, strlen(message));
                    }
                    i = searchMsg(from_user,&id, message, user_name);
                }

            }
            break;
        }
        //Disconnect
        else if(strncmp(buffer,"DISCONNECT",10)==0){
            msg = readLine(sc, buffer, MAX_LINE);
            res=disconnect_user(buffer,sc);
            if (res==0){
                close(client_sc);
            }
            close(sc);
            break;
        }
        //Send message to someone
        else if(strncmp(buffer,"SEND",4)==0){
            char buffer2[MAX_LINE],port[MAX_LINE];
            msg = readLine(sc, buffer, MAX_LINE);
            msg = readLine(sc, buffer2, MAX_LINE);
            msg = readLine(sc, port, MAX_LINE);
            res=send_req_user(buffer,buffer2,port,sc);
            close(sc);
            break;
        }
        else{
            break;
        }
    }
    pthread_exit(NULL);
}

//MAIN
int main(int argc, char *argv[])
{
        //Port number as an argument
        if (argc > 3 || argc < 3 || strcmp(argv[1],"-p")!=0)
        {
            printf("s> enter 1 argument only in  a form: eg.\"./server -p <port_number>\"\n");
            return -1;
        }
    
        server_port = atoi(argv[2]);
    
        printf("s> init server (localhost) 127.0.0.1 : %i\ns>\n",server_port); //GET LOCAL IP SOMEHOW!
    
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
        server_addr.sin_port = htons(server_port);
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
            int sc = accept(sd,(struct sockaddr *)&client_addr,&size);
            pthread_create(&thread,&attr,(void * _Nullable) manage_request,&sc);
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

int addCon(char* id, int port,char* ip)
{
    struct connected* new = (struct connected*)malloc(sizeof(struct connected));
    strcpy(new->id,id);
    new->port = port;
    strcpy(new->ip,ip);
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
int addMsg(char* p_msg, char* p_from, char* p_to,unsigned int p_msg_id)
{
    struct messages* new = (struct messages*)malloc(sizeof(struct messages));
    strcpy(new->msg,p_msg);
    strcpy(new->from,p_from);
    strcpy(new->to,p_to);
    new->msg_id = p_msg_id;
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
            //now we need to delete this message from the message list
            if(prev!=NULL)
                prev->pNext = tmp->pNext;
            else
                pHeadMsg = tmp->pNext;
            free(tmp);
            return 1; //element exists!
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
