
// Functions to get and set count for number of different users created in ERP

#ifndef ACCOUNT_COUNT_H
#define ACCOUNT_COUNT_H
int set_count(int type){
    int fd = open("records/account_count",O_RDWR);
    if (fd == -1) {
        exit(1);
    }
    int offset=lseek(fd,0*sizeof(struct record),SEEK_SET);
    if(offset != -1){
        // Implementing write lock

            struct flock lock;
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            lock.l_pid = getpid();
            int f = fcntl(fd, F_SETLKW, &lock);
            if(f==-1){
                perror("Error obtaining write lock on Count Record!");
                close(fd);
            }
            struct record rec;
            read(fd,&rec,sizeof(rec));
            lseek(fd,0*sizeof(struct record),SEEK_SET);
            
            if(type == 1)
                rec.student++;
            else if(type == 2)
                rec.teacher++;
            else if(type == 3) 
                rec.courses++;
            else rec.enrollments++;
            
            write(fd,&rec,sizeof(rec));
            // Unlocking
            lock.l_type = F_UNLCK;
            fcntl(fd,F_SETLK,&lock);
            close(fd);
            if(type == 1)
                return rec.student;
            else if(type == 2)
                return rec.teacher;
            else if(type == 3) 
                return rec.courses;
            else return rec.enrollments;
    }
}

int get_count(int type){
    int fd = open("records/account_count",O_RDONLY);
    if (fd == -1) {
        exit(1);
    }
    int offset=lseek(fd,0*sizeof(struct record),SEEK_SET);
    if(offset != -1){
        // Implementing write lock

            struct flock lock;
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
            lock.l_len = 0;
            lock.l_pid = getpid();
            int f = fcntl(fd, F_SETLKW, &lock);
            if(f==-1){
                perror("Error obtaining Read lock on Count Record!");
                close(fd);
            }
            struct record rec;
            read(fd,&rec,sizeof(rec));
            // Unlocking
            lock.l_type = F_UNLCK;
            fcntl(fd,F_SETLK,&lock);
            close(fd);
            if(type == 1)
                return rec.student;
            else if(type == 2)
                return rec.teacher;
            else if(type == 3) 
                return rec.courses;
            else return rec.enrollments;           
    }
}

#endif