
// Function to get the Enrollment record for a student in a particular course

#ifndef ENROLLMENT_H
#define ENROLLMENT_H

struct Enrollment get_student_enrollment(int client_socket, int sid, int cid){
    int fd = open("records/enrollments",O_RDWR|O_APPEND);
    struct Enrollment e={0,0,0,0,0};
    if (fd == -1) {
        exit(1);
    }
    if(get_count(3)<cid){
        wr(client_socket,"Wrong Course id..#\n",20);
        return e;
    }
    if(get_count(1)<sid){
        wr(client_socket,"Wrong Student id..#\n",21);
        return e;
    }
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    int f = fcntl(fd, F_SETLKW, &lock);
    if(f==-1){
        perror("Error obtaining Read lock on Enrollments Record!");
        close(fd);
    }
    lseek(fd,0,SEEK_SET);
    int bytesRead;

    while((bytesRead=read(fd,&e,sizeof(struct Enrollment))) == sizeof(struct Enrollment) || bytesRead == -1){
        if(e.studentID == sid && e.courseID == cid){
            break;
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
    close(fd);
    return e;
}

#endif