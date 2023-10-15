
// Functions to handle Login and other operations of Student Module

#ifndef STUDENT_H
#define STUDENT_H

int student_login(int client_socket,char *username, char *password,int *session){
    struct Student ad;
    int fd = open("records/student_details",O_RDONLY);
    if (fd == -1) {
        perror("Failed to open the file");
        exit(1);
    }
    int id;
    char *substr = strstr(username,"MT");
    if(substr){
        if(sscanf(substr+2,"%d",&id)==1){
            int offset=lseek(fd,(id - 1)*sizeof(struct Student),SEEK_SET);
            if(offset!=-1){
                // Implementing read lock
                    struct flock lock;
                    lock.l_type = F_RDLCK;
                    lock.l_whence = SEEK_SET;
                    lock.l_start = (id - 1)*sizeof(struct Student);
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
                                if(get_count(1)+1>id){
                                    if(strcmp(ad.username,username)!=0 || strcmp(ad.password,password)!=0 ){
                                        wr(client_socket,"Username or Password wrong.....#\n",34);
                                        return 0;
                                    }
                                    else if(strcmp(ad.username,username)==0 && strcmp(ad.password,password)==0){
                                        if(ad.isActive){
                                            *session = id;
                                            wr(client_socket,"Successfully Authenticated as Student....~\n",44);
                                            return 1;
                                        }
                                        else wr(client_socket,"Your ID is Blocked.....#\n",26);

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

void view_all_courses(int);
void enroll_course(int,int);
void drop_course(int,int);
int * get_enrollments(int,int,int *,int);

void handle_student(int client_socket, int session){
    char resp[4];
    int choice;
    rd(client_socket,resp,4);
    if(strchr(resp,'~')!=NULL){
        
    }
    while (1)
    {
        wr(client_socket,STUDENT_MENU_STR,STUDENT_MENU_LEN+1);
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
                    view_all_courses(client_socket);
                    break;
                case 2:
                    enroll_course(client_socket,session);
                    break;
                case 3:
                    drop_course(client_socket,session);
                    break;
                case 4:
                    // view enrollments
                    int cnt;
                    char skip[2];
                    get_enrollments(client_socket,session,&cnt,1);
                    if(cnt == 0){
                        wr(client_socket,"No Courses Enrolled...~\n",26);
                        rd(client_socket,skip,2);
                    }
                    break;
                case 5: 
                    wr(client_socket,"Exiting...#\n",13);
                    return;
                    break;
                default:
                    printf("Invalid choice. Please select a valid option (1-9).\n");
                }

        }
    }
}

void view_all_courses(int client_socket){
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
    int course_cnt = get_count(3);
    if(course_cnt == 0){
        wr(client_socket,"No Courses Available...~\n",26);
    }
    struct Course c;
    int bytesRead;
    while((bytesRead=read(fd,&c,sizeof(struct Course))) == sizeof(struct Course) || bytesRead == -1){
        if(c.isActive){
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

void enroll_course(int client_socket,int session){
    int fd = open("records/enrollments",O_RDWR|O_APPEND);
    if (fd == -1) {
        exit(1);
    }
    int course_cnt = get_count(3);
    if(course_cnt == 0){
        wr(client_socket,"No Courses Available...~\n",26);
        return;
    }
    int id;
    char buff[10];
    while(1){
        wr(client_socket,"Enter the course ID number to Enroll: ",39);
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
    int cnt;
    int *enrolled = get_enrollments(client_socket,session,&cnt,0);
    int flag = 0; // Flag to indicate if the number is found
    for (int i = 0; i < cnt; i++) {
        if (enrolled[i] == id) {
            flag = 1;
            break; // Number is found, exit the loop
        }
    }
    if(flag == 1){
        wr(client_socket,"Course already Enrolled...\n~\n",30);
        return ;
    }
    else{
        struct flock lock;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_END;
        lock.l_start = 0;
        lock.l_len = sizeof(struct Student);
        lock.l_pid = getpid();
        int f = fcntl(fd, F_SETLKW, &lock);
        if(f==-1){
            perror("Error obtaining write lock on Student Record!");
            close(fd);
        }
        struct Course c = get_courses(client_socket,id);
        if(c.available > 0){
            int enrollId = set_count(4);
            set_availability(client_socket,id,-1);
            struct Enrollment e = {session,c.id,c.profId,enrollId,1};
            write(fd,&e,sizeof(struct Enrollment));
            char tempBuffer[100];
            sprintf(tempBuffer,"\nSuccessfully Enrolled in the course - %s\n~\n",c.cname);
            wr(client_socket,tempBuffer,strlen(tempBuffer)+1);
        }

        else{
            char skip[2];
            wr(client_socket,"No seats left...\n~\n",20);
            rd(client_socket,skip,2);
        }
        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLK,&lock);
    }
    close(fd);



}

int * get_enrollments(int client_socket, int session, int *cnt,int flag){
    int fd = open("records/enrollments",O_RDONLY);
    if (fd == -1) {
        exit(1);
    }
    int course_cnt = get_count(3);
    if(course_cnt == 0){
        wr(client_socket,"No Courses Available...~\n",26);
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
    struct Enrollment e;
    int bytesRead;
    int arraySize =1;
    int count = 0;
    int *cid = (int *)malloc(arraySize * sizeof(int));
    while((bytesRead=read(fd,&e,sizeof(struct Enrollment))) == sizeof(struct Enrollment) || bytesRead == -1){
       
        if(e.isEnrolled && e.studentID == session){
            char send[100+sizeof(struct Course)],skip[2];
            struct Course c = get_courses(client_socket,e.courseID);
            if(flag){
                sprintf(send,"Course Name: %s\nDepartment: %s\nCredits: %d\nTotal seats: %d\nAvailable Seats: %d\nId: %d\n~\n",c.cname,c.department,c.credits,c.seats,c.available,c.id);
                wr(client_socket,send,strlen(send)+1);
                rd(client_socket,skip,2);
                memset(send,0,100+sizeof(struct Course));
            }
            arraySize += 1;
            cid = (int *)realloc(cid, arraySize * sizeof(int));
            if (cid == NULL) {
                perror("Memory reallocation failed");
            }
            cid[count++] = e.courseID;
        }
    }
    
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
    close(fd);
    *cnt = count;
    return cid;

}

void drop_course(int client_socket, int session){
    int fd = open("records/enrollments",O_RDWR);
    if (fd == -1) {
        exit(1);
    }
    int course_cnt = get_count(3);
    if(course_cnt == 0){
        wr(client_socket,"No Courses Available...~\n",26);
        return;
    }
    int cnt;
    int *enrolled = get_enrollments(client_socket,session,&cnt,0);
    if(cnt == 0){
        wr(client_socket,"No Courses Enrolled...~\n",26);
        return;
    }
    int cid;
    char buff[10];
    struct flock lock;
    while(1){
        wr(client_socket,"Enter the course ID number to Enroll: ",39);
        memset(buff,0,10);
        rd(client_socket,buff,10);
        if(buff[0] == '~'){
            continue;
        }
        if(!isnum(buff)){
                wr(client_socket,"Wrong id entered, Try again...~\n",33);
        }
        else{
            cid = atoi(buff);
            if(cid<=0 || get_count(3)<cid){
                wr(client_socket,"Wrong id entered, Try again...~\n",33);
            }
            else{break;}
        }
    }
    int flag = 0; // Flag to indicate if the number is found
    for (int i = 0; i < cnt; i++) {
        if (enrolled[i] == cid) {
            flag = 1;
            break; // Number is found, exit the loop
        }
    }
    if(flag == 0){
        wr(client_socket,"Course not Enrolled...\n~\n",26);
        return ;
    }
    else{
        struct Enrollment e = get_student_enrollment(client_socket,session,cid);
        int id = e.enrollmentID;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = (id-1)*sizeof(struct Enrollment);
        lock.l_len = sizeof(struct Enrollment);
        lock.l_pid = getpid();
        int f = fcntl(fd, F_SETLKW, &lock);
        if(f==-1){
            perror("Error obtaining write lock on Enrollment Record!");
            close(fd);
        }
        if(e.isEnrolled){
            set_availability(client_socket,id,1);
            e.isEnrolled=0;
            lseek(fd,(id-1)*sizeof(struct Enrollment),SEEK_SET);
            write(fd,&e,sizeof(struct Enrollment));
        }      
    }
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
    close(fd);
}


#endif