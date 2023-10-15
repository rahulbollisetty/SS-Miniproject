
// Client Program to connect to server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#define MAX_MESSAGE_SIZE 4096

void handle_conn(int client_socket);

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_MESSAGE_SIZE];

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(16000); // Use the same port as the server
    server_addr.sin_addr.s_addr = INADDR_ANY; // Server IP address

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");
    int count = 1;
    handle_conn(client_socket);
// Connect client socket sd to server
// connect(client_socket, (void*) (&server_addr), sizeof(server_addr));
// Send message to server
    // Close the client socket
    close(client_socket);

    return 0;
}

void handle_conn(int client_socket){
    char buffer[MAX_MESSAGE_SIZE],dumpBuffer[MAX_MESSAGE_SIZE];
    int writeBytes ,bytes_received,len;
    while(1){
        memset(buffer, 0, sizeof(buffer));
        memset(dumpBuffer, 0, sizeof(dumpBuffer));
        bytes_received = read(client_socket, buffer, sizeof(buffer));
        if (bytes_received <= 0) {
            // Client disconnected or an error occurred
            perror("Error while reading from client socket!");
            close(client_socket);
            return;
        }
        else if(strchr(buffer,'#')!=NULL){
            // server err msg, closing its conn to the client
            strncpy(dumpBuffer, buffer, strlen(buffer) - 2);
            printf("%s\n", dumpBuffer);
            fflush( stdout);
            printf("Closing connection to the server........\n");
            break;
        }
         else if(strchr(buffer,'~')!=NULL){ //skip inp
            // server err msg, closing its conn to the client
            
            strncpy(dumpBuffer, buffer, strlen(buffer) - 2);
            printf("%s\n", dumpBuffer);
            fflush( stdout);
            printf("\n");
            writeBytes = write(client_socket,"~",1);
            if (writeBytes == -1)
            {
                perror("Error while writing to server!");
                break;
            }
        }
        else{
           char writeBuffer[MAX_MESSAGE_SIZE];
            memset(writeBuffer, 0, sizeof(writeBuffer));
            if(strchr(buffer,'!')!=NULL){
                strncpy(dumpBuffer, buffer, strlen(buffer) - 1);
                printf("%s", dumpBuffer); 
                fflush( stdout);
                char password[100];
                strcpy(writeBuffer,getpass(password));
            }
            else{
                printf("%s",buffer);
                fflush( stdout);
                scanf("%[^\n]%*c", writeBuffer);
                printf("\n");
                // if(strlen(writeBuffer)==0)break;
            }
            writeBytes = write(client_socket, writeBuffer, strlen(writeBuffer));
            if (writeBytes <= 0)
            {
                perror("Error while writing to server!");
                exit(EXIT_FAILURE);
            }          
        }  
    
    }
}