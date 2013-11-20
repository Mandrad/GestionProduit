/*
    Gestion de produit -- Server
        - DUBOIS Sebastien
        - MORIN Louis
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h> 
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>    
#include<pthread.h>
  
struct produit
    {
           char ref [10];
           char intitule [10];
           char prix [5];
           char nbexem [3];
    };

// Fonction permettant de créer les threads
void *connection_handler(void *);
  
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    char* messAccueil;
    char* messMenu;
    struct sockaddr_in server , client;
    
    messAccueil = "+---------------------------------------------+\n|Bienvenue dans le service de gestion de stock|\n+---------------------------------------------+\n";
    messMenu =    "          |En attente de connexion|\n          +-----------------------+\n";
      
    system("clear");
    printf("%s",messAccueil);
    sleep(1);
    // Creation de la socket
    printf("...\n--> Creation de la socket\n");
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    sleep(1);
    if (socket_desc == -1)
    {
        printf("--> Impossible de créer la socket");
    }
    printf("--> Socket actif\n");
      
    //Creation de la structure de connexion
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8896 );
    sleep(1);
    printf("--> Liaison de la socket\n");
    sleep(1);
    //Liaison de la socket
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("--> Echec de la liaison");
        sleep(3);system("clear");return 1;
    }
    printf("--> Liaison etablie\n");
    sleep(2);
    
    system("clear");
    printf("%s",messAccueil);
    printf("%s\n",messMenu);
    
    //Demarrage de l'écoute sur la socket
    listen(socket_desc , 3);

    printf("En attente de connections...\n");      
      
    //Acceptation de la connexion
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        printf("...\n--> Connection acceptee\n");
          
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
          
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("--> Thread impossible\n");
            sleep(3);system("clear");return 1;
        }
 
        printf("--> Thread assigné\n");
    }
      
    if (client_sock < 0)
    {
        perror("--> Echec de l'acceptation du client\n");
        sleep(3);system("clear");return 1;
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
        strcpy(res.intitule,"Non trouvé");
    }
    fflush(fichier);
    fclose(fichier);
    return res;
};
  
/*
 * On maintiens la connexion pour chaque client
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
                //Envoi d'un message au client
                struct produit prod;
                memset(prod.ref, 0, sizeof(prod.ref));
                memset(prod.intitule, 0, sizeof(prod.intitule));
                memset(prod.prix, 0, sizeof(prod.prix));
                memset(prod.nbexem, 0, sizeof(prod.nbexem));
                memset(ref, 0, sizeof(ref));
                message = "Référence du produit : ";
                write(sock , message , strlen(message));
                memset(&message, 0, sizeof(message));
                //Reception du message client
                if( (read_size = recv(sock , ref , 2000 , 0)) > 0 )
                {   
                    int existe=Existe(ref);
                    if(existe == 0)
                    {
                        strcpy(prod.ref,ref);
                        message = "Intitulé du produit : ";
                        //Envoi d'un message au client
                        write(sock , message , strlen(message));
                        memset(&message, 0, sizeof(message));
                        if( (read_size = recv(sock , prod.intitule , 2000 , 0)) > 0 )
                        {
                            message = "Prix : ";
                            //Envoi d'un message au client
                            write(sock , message , strlen(message)+1);
                            memset(&message, 0, sizeof(message));
                            if( (read_size = recv(sock , prod.prix , 2000 , 0)) > 0 )
                            {
                                message = "Nombre d'exemplaires : ";
                                //Envoi d'un message au client
                                write(sock , message , strlen(message)+1);
                                memset(&message, 0, sizeof(message));
                                if( (read_size = recv(sock , prod.nbexem , 2000 , 0)) > 0 )
                                {
                                    EnregDansFichier(prod);
                                    message = "~";
                                    //Envoi d'un message au client
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
                        //Envoi d'un message au client
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
                //Envoi au client
                message = "Référence du produit (Search) : ";
                write(sock , message , strlen(message));
                memset(&message, 0, sizeof(message));
                //Reception du message client
                if( (read_size = recv(sock , ref , 2000 , 0)) > 0 )
                { 
                    prod = Recherche(ref);
                    message = prod.intitule;
                    //Envoi du message de retour au client
                    write(sock , message , strlen(message)+1);
                    memset(&message, 0, sizeof(message));
                    message = "~";
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
                //Envoi des messages au clients
                message = "Référence du produit : ";
                write(sock , message , strlen(message));  
                memset(&message, 0, sizeof(message));
                //Reception des messages depuis le client
                if( (read_size = recv(sock , ref , 2000 , 0)) > 0 )
                {
                    if(Existe(ref)==1)
                    {
                        message = "Nombre à rajouter : ";                    
                        //Envoi au client
                        write(sock , message , strlen(message));
                        memset(&message, 0, sizeof(message));
                        if( (read_size = recv(sock , nbexem , 2000 , 0)) > 0 )
                        {
                            int refarajouter;
                            refarajouter = atoi(nbexem);
                            int refpossible;
                            //Recherche dans le fichier
                            prod = Recherche(ref);
                            refpossible = atoi(prod.nbexem);
                            AugmenterStock(ref,refpossible+refarajouter);
                            message = "~";

                            //Renvoi le message au client
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
            //Envoi le message au client
            write(sock , message , strlen(message)+1);
            memset(&message, 0, sizeof(message));
            break;
        default:
            message = "~";
            //Envoi le message au client
            write(sock , message , strlen(message)+1);
            memset(&message, 0, sizeof(message));
            break;
        }
        
    }
      
    if(read_size == 0)
    {
        printf("--> Deconnection du client\n");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("--> Echec de la reception\n");
    }
          
    //Libère la socket
    free(socket_desc);
    
      
    return 0;
}
