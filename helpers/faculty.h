#ifndef FACULTY_H
#define FACULTY_H

void add_course(int client_socket, int session);
void view_courses(int , int);
void delete_course(int,int);

int faculty_login(int client_socket,char *username, char *password,int *session){
    struct Teacher ad;
    int fd = open("records/teacher_details",O_RDONLY);
    if (fd == -1) {
        perror("Failed to open the file");
        exit(1);
    }
    int id;
    char *substr = strstr(username,"PROF");
    if(substr){
        if(sscanf(substr+4,"%d",&id)==1){
            int offset=lseek(fd,(id - 1)*sizeof(struct Teacher),SEEK_SET);

            if(offset!=-1){
                // Implementing read lock
                    struct flock lock;
                    lock.l_type = F_RDLCK;
                    lock.l_whence = SEEK_SET;
                    lock.l_start = (id - 1)*sizeof(struct Teacher);
                    lock.l_len = 0;
                    lock.l_pid = getpid();
                    int f = fcntl(fd, F_SETLKW, &lock);
                    if(f==-1){
                        perror("Error obtaining read lock on Admin Record!");
                        close(fd);
                        return 0;
                    }
                    int readBytes=read(fd,&ad,sizeof(ad));
                    // Unlocking
                    lock.l_type = F_UNLCK;
                    fcntl(fd,F_SETLK,&lock);
                    close(fd);
                    if(readBytes > 0){
                                if(get_count(2)+1>id){
                                    if(strcmp(ad.username,username)!=0 || strcmp(ad.password,password)!=0 ){
                                        wr(client_socket,"Username or Password wrong.....#\n",34);
                                        return 0;
                                    }
                                    else if(strcmp(ad.username,username)==0 && strcmp(ad.password,password)==0){
                                        *session = ad.id;
                                        wr(client_socket,"Successfully Authenticated as Faculty....~\n",42);
                                        return 1;
                                    }
                                }
                                else wr(client_socket,"Username not found.....#\n",26);
                        }
                        else wr(client_socket,"Username wrong.....#\n",22);                       
            }

        }
        else wr(client_socket,"Username wrong.....#\n",22);
    }
    else wr(client_socket,"Username wrong.....#\n",22);
    return 0;
}



void handle_faculty(int client_socket, int session){
    char resp[4];
    int choice;
    rd(client_socket,resp,4);
    if(strchr(resp,'~')!=NULL){
        
    }
    while (1)
    {
        wr(client_socket,TEACHER_MENU_STR,TEACHER_MENU_LEN+1);
        memset(resp,0,4);
        if(rd(client_socket,resp,4)==0){
            wr(client_socket,"No input try again...#\n",24);
            break;
        }
        if(resp[0] == '~'){

        }
        else if(!isnum(resp)){
            wr(client_socket,"Wrong option entered...~\n",26);
            // break;
        }
        else{
            choice = atoi(resp);
            switch (choice) {
                case 1:
                    view_courses(client_socket,session);
                    break;
                case 2:
                    add_course(client_socket,session);
                    break;
                case 3:
                    delete_course(client_socket,session);
                    break;
                case 4:
                    // modify(client_socket,1);
                    break;
                case 5:
                    // view enrollments
                    break;
                case 6:
                    // change_password(client_socket,2);
                    break;
                case 7:
                    wr(client_socket,"Exiting...#\n",13);
                    return;
                    break;
                default:
                    printf("Invalid choice. Please select a valid option (1-9).\n");
                }

        }
    }
    
}

void add_course(int client_socket, int session){
    int fd = open("records/courses",O_RDWR|O_APPEND);
        if (fd == -1) {
            exit(1);
        }
        // Implementing write lock
        struct flock lock;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_END;
        lock.l_start = 0;
        lock.l_len = sizeof(struct Course);
        lock.l_pid = getpid();
        int f = fcntl(fd, F_SETLKW, &lock);
        if(f==-1){
            perror("Error obtaining write lock on Course Record!");
            close(fd);
        }
        int id = set_count(3);
        struct Course c;
        char seats[4],credits[2];
        wr(client_socket,"Enter Name: ",13);
        rd(client_socket,c.cname,sizeof(c.cname));
        wr(client_socket,"Enter Department: ",19);
        rd(client_socket,c.department,sizeof(c.department));
        wr(client_socket,"Enter Credits: ",16);
        rd(client_socket,credits,2);
        wr(client_socket,"Enter Total Seats: ",20);
        rd(client_socket,seats,4);
        c.credits = atoi(credits);
        c.seats = atoi(seats);
        c.available = c.seats;
        c.profId = session;
        c.isActive = 1;
        c.id = id;
        write(fd,&c,sizeof(struct Course));
        char tempBuffer[100];
        sprintf(tempBuffer,"\nSuccessfully added Course\nCourse-Id Generated is: %d\n~",id);
        wr(client_socket,tempBuffer,strlen(tempBuffer)+1);
        // Unlocking
        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLK,&lock);
        close(fd);

}

void view_courses(int client_socket, int session){
    int fd = open("records/courses",O_RDONLY);
    if (fd == -1) {
        exit(1);
    }
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    int f = fcntl(fd, F_SETLKW, &lock);
    if(f==-1){
        perror("Error obtaining Read lock on Courses Record!");
        close(fd);
    }
    lseek(fd,0,SEEK_SET);
    struct Course c;
    int bytesRead;
    while((bytesRead=read(fd,&c,sizeof(struct Course))) == sizeof(struct Course) || bytesRead == -1){
        if(c.isActive && c.profId==session){
            char send[100+sizeof(struct Course)],skip[2];
            sprintf(send,"Course Name: %s\nDepartment: %s\nCredits: %d\nTotal seats: %d\nAvailable Seats: %d\nId: %d\n~\n",c.cname,c.department,c.credits,c.seats,c.available,c.id);
            wr(client_socket,send,strlen(send)+1);
            rd(client_socket,skip,2);
            memset(send,0,100+sizeof(struct Course));
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
    close(fd);
}

void delete_course(int client_socket, int session){
    int fd = open("records/courses",O_RDWR);
    if (fd == -1) {
        exit(1);
    }
    view_courses(client_socket,session);
    int id;
    char buff[10];
    while(1){
        wr(client_socket,"Enter the course ID number to Deactivate: ",43);
        memset(buff,0,10);
        rd(client_socket,buff,10);
        if(buff[0] == '~'){
            continue;
        }
        if(!isnum(buff)){
                wr(client_socket,"Wrong id entered, Try again...~\n",33);
        }
        else{
            id = atoi(buff);
            if(id<=0 || get_count(3)<id){
                wr(client_socket,"Wrong id entered, Try again...~\n",33);
            }
            else{break;}
        }
    }
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = (id-1)*sizeof(struct Course);
    lock.l_len = sizeof(struct Course);
    lock.l_pid = getpid();
    int f = fcntl(fd, F_SETLKW, &lock);
    if(f==-1){
        perror("Error obtaining write lock on Course Record!");
        close(fd);
    }
    lseek(fd,(id-1)*sizeof(struct Course),SEEK_SET);
    struct Course c;
    read(fd,&c,sizeof(struct Course));
    c.isActive = 0;
    lseek(fd,(id-1)*sizeof(struct Course),SEEK_SET);
    write(fd,&c,sizeof(struct Course));
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
    close(fd);
}

#endif
