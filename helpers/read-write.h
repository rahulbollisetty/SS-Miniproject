
// Helper functions for sending and receiving data to and from client

#ifndef RDWR_H
#define RDWR_H


int rd(int client_socket, char *buffer,int len) {
    memset(buffer,0,len);
    int bytes_received = read(client_socket, buffer, len);
        if (bytes_received <= 0) {
             return 0;
        }
    buffer[bytes_received] = '\0';
}
void wr(int client_socket, const char *buffer, int len) {
    
    int b = write(client_socket,buffer,len);
    // printf("%d\n",b);
}

#endif