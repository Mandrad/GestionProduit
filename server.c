/*
    C socket server example, handles multiple clients using threads
*/
  
#include<stdio.h>
#include<string.h> //strlen
#include<stdlib.h> //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h> //write
#include<pthread.h> //for threading , link with lpthread
  
struct produit
    {
           char ref [10]; //numero du compte
           char intitule [10]; //type du compte: cheque ou epargne
           char prix [5]; //nom du client
           char nbexem [3]; //prenom(s) du client
    };

//the thread function
void *connection_handler(void *);
  
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    
    struct sockaddr_in server , client;
      
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Impossible de créer la socket");
    }
    puts("Socket créée");
      
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8895 );
      
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
      
    //Listen
    listen(socket_desc , 3);

    puts("En attente de connections...");
      
    //Accept and incoming connection
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accéptée");
          
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
          
        if( pthread_create( &sniffer_thread , NULL , connection_handler , (void*) new_sock) < 0)
        {
            perror("Thread impossible");
            return 1;
        }
          
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
      
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
      
    return 0;
};

void ReduireStock(char *ref, int a)
{
    FILE * fichier;
    fichier = fopen("catalogue.dat" , "r+"); //ouverture du fichier en écriture/lecture
    struct produit prod;
    while (fscanf(fichier,"%s\n%s\n%s\n%s\n", prod.ref, prod.intitule, prod.prix, prod.nbexem)!=EOF) // tant que la fin du fichier n'est pas atteint
    {
        if (!strcmp(prod.ref,ref))
        {
            sprintf(prod.nbexem,"%d",a);
         fseek(fichier,-(strlen(prod.ref)+strlen(prod.intitule)+strlen(prod.prix)+strlen(prod.nbexem)+4), SEEK_CUR);        
            fprintf(fichier,"%s\n%s\n%s\n%s\n", prod.ref, prod.intitule, prod.prix, prod.nbexem);
            break;
        }
    }
    fflush(fichier);
    fclose(fichier);
};

int Existe(char *ref)
{
    FILE * fichier; int rep;
    rep = 0;
    fichier = fopen("catalogue.dat" , "ar+"); //ouverture du fichier en écriture/lecture
    if(fichier != NULL)
    {
        struct produit prod;
        while (fscanf(fichier,"%s\n%s\n%s\n%s\n", prod.ref, prod.intitule, prod.prix, prod.nbexem)!=EOF) // tant que la fin du fichier n'est pas atteint
        {
            if (!strcmp(prod.ref,ref))
            {
                rep = 1;
                break;
            }
        }
    }
    fflush(fichier);
    fclose(fichier);
    return rep;
};

void AugmenterStock(char *ref, int a)
{
    FILE * fichier;
    fichier = fopen("catalogue.dat" , "r+"); //ouverture du fichier en écriture/lecture
    struct produit prod;
    while (fscanf(fichier,"%s\n%s\n%s\n%s\n", prod.ref, prod.intitule, prod.prix, prod.nbexem)!=EOF) // tant que la fin du fichier n'est pas atteint
    {
        if (!strcmp(prod.ref,ref))
        {
            sprintf(prod.nbexem,"%d",a);
         fseek(fichier,-(strlen(prod.ref)+strlen(prod.intitule)+strlen(prod.prix)+strlen(prod.nbexem)+4), SEEK_CUR);        
            fprintf(fichier,"%s\n%s\n%s\n%s\n", prod.ref, prod.intitule, prod.prix, prod.nbexem);
            break;
        }
    }
    fflush(fichier);
    fclose(fichier);
};

void EnregDansFichier(struct produit prod)
{
    FILE * fichier;
    fichier = fopen("catalogue.dat" , "at");
    fprintf(fichier,"%s\n%s\n%s\n%s\n", prod.ref,prod.intitule, prod.prix, prod.nbexem);
    fflush(fichier);
    fclose(fichier);
};

struct produit Recherche (char *ref)
{
    FILE * fichier;
    fichier = fopen("catalogue.dat" , "rt");
    rewind(fichier);
    int trouve =0;
    
    struct produit prod,res;
    while (fscanf(fichier,"%s\n%s\n%s\n%s\n", prod.ref, prod.intitule, prod.prix, prod.nbexem)!=EOF) // tant que la fin du fichier n'est pas atteint
    {
        if (!strcmp(prod.ref,ref)) // si un numero de compte a ete saisi
        {
            trouve = 1;
            res = prod;
            break;
        }
    }
    if (trouve == 0)
    {
        strcpy(res.intitule,"Introuvé");
    }
    fflush(fichier);
    fclose(fichier);
    return res;
};
  
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    char ref[10];
    char nbexem[3];
    
    int read_size;
    
    char *message , client_message[2000];
      
    while((read_size = recv(sock , client_message , 2000 , 0)) > 0)
    {
        switch (client_message[0])
        {
        case 'A' :
            while (1)
            {
                //Send some messages to the client
                struct produit prod;
                memset(prod.ref, 0, sizeof(prod.ref));
                memset(prod.intitule, 0, sizeof(prod.intitule));
                memset(prod.prix, 0, sizeof(prod.prix));
                memset(prod.nbexem, 0, sizeof(prod.nbexem));
                memset(ref, 0, sizeof(ref));
                message = "Référence du produit\n";
                write(sock , message , strlen(message));
                memset(&message, 0, sizeof(message));
                //Receive a message from client
                if( (read_size = recv(sock , ref , 2000 , 0)) > 0 )
                {
                    int existe=Existe(ref);
                    if(existe == 0)
                    {
                        strcpy(prod.ref,ref);
                        message = "Intitulé du produit : \n";
                        //Send the message back to client
                        write(sock , message , strlen(message));
                        memset(&message, 0, sizeof(message));
                        if( (read_size = recv(sock , prod.intitule , 2000 , 0)) > 0 )
                        {
                            message = "Prix : \n";
                            //Send the message back to client
                            write(sock , message , strlen(message)+1);
                            memset(&message, 0, sizeof(message));
                            if( (read_size = recv(sock , prod.prix , 2000 , 0)) > 0 )
                            {
                                message = "Nombre d'exemplaires : \n";
                                //Send the message back to client
                                write(sock , message , strlen(message)+1);
                                memset(&message, 0, sizeof(message));
                                if( (read_size = recv(sock , prod.nbexem , 2000 , 0)) > 0 )
                                {
                                    EnregDansFichier(prod);
                                    message = "~";
                                    //Send the message back to client
                                    write(sock , message , strlen(message)+1);
                                    memset(&message, 0, sizeof(message));
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        message = "~";
                        //Send the message back to client
                        write(sock , message , strlen(message)+1);
                        memset(&message, 0, sizeof(message));
                    }
                }
            }
            break;
        case 'R' :
            while (1)
            {
                struct produit prod;
                memset(prod.ref, 0, sizeof(prod.ref));
                memset(prod.intitule, 0, sizeof(prod.intitule));
                memset(prod.prix, 0, sizeof(prod.prix));
                memset(prod.nbexem, 0, sizeof(prod.nbexem));
                memset(ref, 0, sizeof(ref));
                //Send some messages to the client
                message = "Référence du produit (Search)\n";
                write(sock , message , strlen(message));
                memset(&message, 0, sizeof(message));
                //Receive a message from client
                if( (read_size = recv(sock , ref , 2000 , 0)) > 0 )
                {
                    prod = Recherche(ref);
                    message = prod.intitule;
                    //Send the message back to client
                    write(sock , message , strlen(message)+1);
                    memset(&message, 0, sizeof(message));
                    message = "~";
                    write(sock , message , strlen(message)+1);
                    memset(&message, 0, sizeof(message));
                    break;
                }
            }
            break;
        case 'E' :
            while (1)
            {
                struct produit prod;
                memset(prod.ref, 0, sizeof(prod.ref));
                memset(prod.intitule, 0, sizeof(prod.intitule));
                memset(prod.prix, 0, sizeof(prod.prix));
                memset(prod.nbexem, 0, sizeof(prod.nbexem));
                memset(ref, 0, sizeof(ref));
                memset(nbexem, 0, sizeof(nbexem));
                //Send some messages to the client
                message = "Voulez vous télécharger le catalogue ? (O: Oui / N: Non)\n";
                write(sock , message , strlen(message));
                memset(&message, 0, sizeof(message));
                //Receive a message from client
                if( (read_size = recv(sock , nbexem , 2000 , 0)) > 0 )
                    {
                        int refaretirer;
                        refaretirer = atoi(nbexem);
                        int refpossible;
                        //Recherche dans le fichier !!!
                        prod = Recherche(ref);
                        refpossible = atoi(prod.nbexem);
                        if(refaretirer <= refpossible)
                        {
                            ReduireStock(ref,refpossible-refaretirer);
                                                        message = "~";
                        }
                        else
                        {
                            message = "Pas assez";
                        }
                        //Send the message back to client
                        write(sock , message , strlen(message)+1);
                        memset(&message, 0, sizeof(message));
                        break;
                    }
            }
            break;
        case 'N' :
            while (1)
            {
                struct produit prod;
                memset(prod.ref, 0, sizeof(prod.ref));
                memset(prod.intitule, 0, sizeof(prod.intitule));
                memset(prod.prix, 0, sizeof(prod.prix));
                memset(prod.nbexem, 0, sizeof(prod.nbexem));
                memset(ref, 0, sizeof(ref));
                memset(nbexem, 0, sizeof(nbexem));
                //Send some messages to the client
                message = "Référence du produit\n";
                write(sock , message , strlen(message));
                memset(&message, 0, sizeof(message));
                //Receive a message from client
                if( (read_size = recv(sock , ref , 2000 , 0)) > 0 )
                {
                    if(Existe(ref)==1)
                    {
                        message = "Nombre à rajouter: \n";
                        //Send the message back to client
                        write(sock , message , strlen(message));
                        memset(&message, 0, sizeof(message));
                        if( (read_size = recv(sock , nbexem , 2000 , 0)) > 0 )
                        {
                            int refarajouter;
                            refarajouter = atoi(nbexem);
                            int refpossible;
                            //Recherche dans le fichier !!!
                            prod = Recherche(ref);
                            refpossible = atoi(prod.nbexem);
                            AugmenterStock(ref,refpossible+refarajouter);
                            message = "~";

                            //Send the message back to client
                            write(sock , message , strlen(message)+1);
                            memset(&message, 0, sizeof(message));
                            break;
                        }
                    }
                    else
                    {
                        message = "~";
                        write(sock , message , strlen(message)+1);
                        memset(&message, 0, sizeof(message));
                        break;
                    }
                }
            }
            break;
        
        case 'Q' :
            message = "$";
            //Send the message back to client
            write(sock , message , strlen(message)+1);
            memset(&message, 0, sizeof(message));
            break;
        default:
            message = "~";
            //Send the message back to client
            write(sock , message , strlen(message)+1);
            memset(&message, 0, sizeof(message));
            break;
        }
        
    }
      
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
          
    //Free the socket pointer
    free(socket_desc);
    
      
    return 0;
}

    Status
    API
    Training
    Shop
    Blog
    About

    © 2013 GitHub, Inc.
    Terms
    Privacy
    Security
    Contact

