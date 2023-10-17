
// Server Program to connect to multiple client simultaneously in a safe and consistent manner, and handle different operations for each user.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "helpers/menu.h"
#include "helpers/read-write.h"
#include "structures.h"
#include "helpers/getSet_count.h"
#include "helpers/enrollments.h"
#include "helpers/admin.h"
#include "helpers/course.h"
#include "helpers/faculty.h"
#include "helpers/student.h"
#include "helpers/login.h"

#define MAX_CLIENTS 3
#define MAX_MESSAGE_SIZE 4096



int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[MAX_MESSAGE_SIZE];

    int client_count = 0;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(16000);

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Socket binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port 16000...\n");

     while (1) {
        // Accept a new connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Accepting failed");
            continue;
        }
        if (client_count >= MAX_CLIENTS) {
            printf("Client limit reached. Connection refused.\n");
            close(client_socket);
        }
        else{
            // Fork a new process to handle the client
            pid_t child_pid = fork();
            if (child_pid == -1) {
                perror("Fork failed");
                close(client_socket);
                continue;
            }
            if (child_pid == 0) {
                // Child process
                close(server_socket); // Close the server socket in the child process
                if (login(client_socket)==0){

                    exit(EXIT_FAILURE);
                }
                else{
                    exit(EXIT_SUCCESS);
                }

            } else {
                // Parent process
                close(client_socket); // Close the client socket in the parent process
                client_count++;
            }

        }
        int status;
        while (waitpid(-1, &status, WNOHANG) > 0) {
            client_count--;
        }
        
    }

    // Close the server socket (not reached in this example)
    close(server_socket);

    return 0;
}

