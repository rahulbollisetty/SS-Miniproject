
// Functions to handle Login and other operations of Faculty Module

#ifndef FACULTY_H
#define FACULTY_H

void add_course(int client_socket, int session);
int* view_courses(int , int, int *,int);
void delete_course(int,int);
struct Enrollment* view_enrollments(int, int , int *,int);
void modify_course(int , int);
int handle_seats(int , int , int , int, int, int);
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
                    int courseCnt;
                    view_courses(client_socket,session,&courseCnt,1);
                    break;
                case 2:
                    add_course(client_socket,session);
                    break;
                case 3:
                    delete_course(client_socket,session);
                    break;
                case 4:
                    modify_course(client_socket,session);
                    break;
                case 5:
                    int cnt;
                    view_enrollments(client_socket,session,&cnt,1);
                    break;
                case 6:
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

int* view_courses(int client_socket, int session, int *courseCount,int flag){
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
    int arraySize =1;
    int count = 0;
    int *cid = (int *)malloc(arraySize * sizeof(int));
    while((bytesRead=read(fd,&c,sizeof(struct Course))) == sizeof(struct Course) || bytesRead == -1){
        if(c.isActive && c.profId==session){
            if(flag){
            char send[100+sizeof(struct Course)],skip[2];
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
            cid[count++] = c.id;
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
    close(fd);
    *courseCount=count;
    return cid;
}

void delete_course(int client_socket, int session){
    int fd = open("records/courses",O_RDWR);
    if (fd == -1) {
        exit(1);
    }
    int courseCount;
    int *cids = view_courses(client_socket,session,&courseCount,1);
    int enrollCnt;
    struct Enrollment *e = view_enrollments(client_socket,session,&enrollCnt,0);
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
    int flag = 0; // Flag to indicate if the number is found
    for (int i = 0; i < courseCount; i++) {
        if (cids[i] == id) {
            flag = 1;
            break; // Number is found, exit the loop
        }
    }
    if(flag == 0){
        wr(client_socket,"Course not Found...\n~\n",23);
        return ;
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

    
    fd = open("records/enrollments",O_RDWR);
    if (fd == -1) {
        exit(1);
    }
    // Un-enrolling students from deleted course
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    f = fcntl(fd, F_SETLKW, &lock);
    if(f==-1){
        perror("Error obtaining Read lock on Courses Record!");
        close(fd);
    }
    lseek(fd,0,SEEK_SET);    
    for(int i=0;i<enrollCnt;i++){
        struct Enrollment w=e[i];
        if(w.courseID == id){
            int eid = w.enrollmentID;
            w.isEnrolled = 0;
            lseek(fd,(eid-1)*sizeof(struct Enrollment),SEEK_SET);
            write(fd,&w,sizeof(struct Enrollment));
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
    close(fd);
}

struct Enrollment* view_enrollments(int client_socket, int session , int *enrollCount,int flag){
    int fd = open("records/enrollments",O_RDONLY);
    if (fd == -1) {
        exit(1);
    }
    int courseCount=0,arraySize=1,count=0;
    struct Enrollment *e = (struct Enrollment *)malloc(arraySize * sizeof(struct Enrollment));
    
    int *cids=view_courses(client_socket,session,&courseCount,0);
    if(courseCount == 0){
        wr(client_socket,"No Courses Created...\n~\n",25);
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
        perror("Error obtaining Read lock on Courses Record!");
        close(fd);
    }
    int bytesRead;
    for(int i=0;i<courseCount;i++){
        struct Course c = get_courses(client_socket,cids[i]);
        char send[100+sizeof(struct Course)],skip[2];
        if(flag){
                sprintf(send,"Course Name: %s\nDepartment: %s\nCredits: %d\nTotal seats: %d\nAvailable Seats: %d\nId: %d\n~\n",c.cname,c.department,c.credits,c.seats,c.available,c.id);
                wr(client_socket,send,strlen(send)+1);
                rd(client_socket,skip,2);
                memset(send,0,100+sizeof(struct Course));
            }
        struct Enrollment er;
        lseek(fd,0,SEEK_SET);
        int ec=0;
        while((bytesRead=read(fd,&er,sizeof(struct Enrollment))) == sizeof(struct Enrollment) || bytesRead == -1){
            if(er.isEnrolled && er.courseID == c.id && er.profID == session){
                char sk[2];
            if(flag){
                wr(client_socket,".........................Enrollment Details.........................\n~\n",72);
                view_participant(client_socket,1,er.studentID);
                rd(client_socket,sk,2);
            }
            arraySize += 1;
            e = (struct Enrollment *)realloc(e, arraySize * sizeof(struct Enrollment));
            if (e == NULL) {
                perror("Memory reallocation failed");
            }
            e[count++] = er;
            ec++;
            }
        }
        if(ec==0){
            char sk[2];
            wr(client_socket,".........................Enrollment Details.........................\n~\n",72);
            wr(client_socket,"No Enrollments for this course...\n~\n",37);
            rd(client_socket,sk,2);
        }
    }

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
    close(fd);
    *enrollCount = count;
    return e; 
}

void modify_course(int client_socket, int session){

    int courseCount;
    int *cids = view_courses(client_socket,session,&courseCount,1);
    
    int id;
    char buff[10];
    while(1){
        wr(client_socket,"Enter the course ID number to Modify: ",39);
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
    int flag = 0; // Flag to indicate if the number is found
    for (int i = 0; i < courseCount; i++) {
        if (cids[i] == id) {
            flag = 1;
            break; // Number is found, exit the loop
        }
    }
    if(flag == 0){
        wr(client_socket,"Course not Found...\n~\n",23);
        return ;
    }
    int fd = open("records/courses",O_RDWR);
    if (fd == -1) {
        exit(1);
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
    char resp[2];
    int choice;
    while(1){
        wr(client_socket,"Enter option{1. Course Name, 2. Department, 3. Credits, 4. Seats}: ",68);
        memset(resp,0,2);
        rd(client_socket,resp,2);
        if(resp[0] == '~'){
            continue;
        }
        if(!isnum(resp)){
                wr(client_socket,"Wrong option entered, Try again...~\n",37);
        }
        else{
            choice = atoi(resp);
            if(choice > 4 || choice < 1){
                wr(client_socket,"Wrong option entered, Try again...~\n",37);
            }
            else{break;}
        }
    }
    char credits[4];
    switch (choice)
        {
        case 1:
            wr(client_socket,"Enter Course Name: ",20);
            rd(client_socket,c.cname,sizeof(c.cname));
            break;
        case 2:
            wr(client_socket,"Enter Department: ",19);
            rd(client_socket,c.department,sizeof(c.department));
            break;
        case 3: 
            wr(client_socket,"Enter Credits: ",16);
            rd(client_socket,credits,sizeof(credits));
            c.credits = atoi(credits);
            break;
        case 4:
            char s[4];
            wr(client_socket,"Enter Seats: ",14);
            rd(client_socket,s,sizeof(s));
            int seats = atoi(s);
            int retV = handle_seats(client_socket,session,id,c.seats,seats,c.available);
            c.available +=  retV;
            c.seats = seats;
            break;
        default:
            break;
        }
        lseek(fd,(id-1)*sizeof(struct Course),SEEK_SET);
        write(fd,&c,sizeof(struct Course));

        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLK,&lock);
        close(fd);
}

int handle_seats(int client_socket, int session,int cid, int prevSeats, int currSeats, int available){
    if(prevSeats <= currSeats){
        return currSeats-prevSeats;
    } 
    else if(prevSeats > currSeats){

        int seatsDecr = prevSeats - currSeats;
        if(available >= seatsDecr)return -seatsDecr;
        else{
        int unEnrollNum = seatsDecr - available;
        int enrollCnt;
        struct Enrollment *e = view_enrollments(client_socket,session,&enrollCnt,0);
        int struct_size = sizeof(struct Enrollment);

        int fd = open("records/enrollments",O_RDWR);
        if (fd == -1) {
            exit(1);
        }
        // Un-enrolling students from deleted course
        struct flock lock;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;
        lock.l_pid = getpid();
        int f = fcntl(fd, F_SETLKW, &lock);
        if(f==-1){
            perror("Error obtaining Read lock on Courses Record!");
            close(fd);
        }

        int file_size = lseek(fd,0,SEEK_END);

        if (file_size == -1) {
            perror("Error getting file size");
            lock.l_type = F_UNLCK;
            fcntl(fd,F_SETLK,&lock);
            close(fd);
            return -2;
        }
        for (int i = file_size - struct_size; i >= 0; i -= struct_size) {
            lseek(fd, i, SEEK_SET);  // Seek to the current position
            if(unEnrollNum>0){
                struct Enrollment unEnroll;
                if (read(fd, &unEnroll, struct_size) == -1) {
                    perror("Error reading the file");
                    lock.l_type = F_UNLCK;
                    fcntl(fd,F_SETLK,&lock);
                    close(fd);
                    return 1;
                }
                struct Enrollment w = unEnroll;
                if(w.courseID == cid){
                    int eid = w.enrollmentID;
                    w.isEnrolled = 0;
                    lseek(fd,(eid-1)*sizeof(struct Enrollment),SEEK_SET);
                    write(fd,&w,sizeof(struct Enrollment));
                }
                unEnrollNum--;
            }
            else break;
        }

        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLK,&lock);
        close(fd);
        return -available;
        }
    }
}
#endif
