
// Progam to set the initial count numbers of all types of users, courses and enrollments in account_count

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include<stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "../structures.h"


int main() {
    struct AdminCredentials admin;
    strcpy(admin.username,"root");
    strcpy(admin.password,"root");

    // Open the flat file in write mode
    int fd = open("../records/admin_cred", O_RDWR);

    int bytes_written = write(fd, &admin, sizeof(struct AdminCredentials));

    if (bytes_written == -1) {
        perror("Failed to write to the file");
        close(fd);
        exit(1);
    }

    // Close the file
    close(fd);
    
    // Setting initial value of account counts as 0
    fd = open("../records/account_count", O_RDWR);
    struct record rec={0,0,0,0};
    bytes_written = write(fd, &rec, sizeof(rec));

    if (bytes_written == -1) {
        perror("Failed to write to the file");
        close(fd);
        exit(1);
    }

    // Close the file
    close(fd);



    return 0;
}
