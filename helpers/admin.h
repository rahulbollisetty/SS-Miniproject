
// Functions to handle Login and other operations of Admin Module


#ifndef ADMIN_H
#define ADMIN_H


bool isnum(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

void add_student(int client_socket);
void add_teacher(int client_socket);
void view_participant(int ,int,int);
void activate_deactivate(int,int);
void modify(int,int);
// Funtion to handle login
int admin_login(int client_socket,char *username, char *password){
    struct AdminCredentials ad;
    int fd = open("records/admin_cred",O_RDONLY);
    if (fd == -1) {
        perror("Failed to open the file");
        exit(1);
    }
    int offset=lseek(fd,0*sizeof(struct AdminCredentials),SEEK_SET);
    if(offset!=-1){
        // Implementing read lock
            struct flock lock;
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 0;
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
                if(strcmp(ad.username,username)!=0 || strcmp(ad.password,password)!=0){
                    wr(client_socket,"Username or Password wrong.....#\n",34);
                    return 0;
                }
                else if(strcmp(ad.username,username)==0 && strcmp(ad.password,password)==0){
                    wr(client_socket,"Successfully Authenticated as Admin....~\n",42);
                    return 1;
                }
            }
            return 0;
    }
}

// Function to handle admin
void handle_Admin(int client_socket) {
    char resp[4];
    int choice;
    rd(client_socket,resp,4);
    if(strchr(resp,'~')!=NULL){
        
    }
    while (1)
    {
        wr(client_socket,ADMIN_MENU_STR,ADMIN_MENU_LEN+1);
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
                    add_student(client_socket);
                    break;
                case 2:
                    view_participant(client_socket,1,-1);
                    break;
                case 3:
                    add_teacher(client_socket);
                    break;
                case 4:
                    view_participant(client_socket,2,-1);
                    break;
                case 5:
                    activate_deactivate(client_socket,1);
                    break;
                case 6:
                    activate_deactivate(client_socket,0);
                    break;
                case 7:
                    modify(client_socket,1);
                    break;
                case 8:
                    modify(client_socket,2);
                    break;
                case 9:
                    wr(client_socket,"Exiting...#\n",13);
                    return;
                    break;
                default:
                    printf("Invalid choice. Please select a valid option (1-9).\n");
                }

        }


    }  
}
void add_student(int client_socket){
    
    int fd = open("records/student_details",O_RDWR|O_APPEND);
        if (fd == -1) {
            exit(1);
        }
        // Implementing write lock
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
        int id = set_count(1);
        struct Student st;
        char age[2],gen[2];
        sprintf(st.username,"MT%d",id);
        wr(client_socket,"Enter Name: ",13);
        rd(client_socket,st.name,sizeof(st.name));
        wr(client_socket,"Enter Email: ",14);
        rd(client_socket,st.email,sizeof(st.email));
        wr(client_socket,"Enter Address: ",16);
        rd(client_socket,st.address,sizeof(st.address));
        wr(client_socket,"Enter Gender: ",15);
        rd(client_socket,gen,sizeof(gen));
        st.gender = gen[0];
        wr(client_socket,"Enter Age: ",12);
        rd(client_socket,age,sizeof(age));
        st.age = atoi(age);
        st.rollNumber = id;
        st.isActive = true;
        strcpy(st.password,"iiitb");
        write(fd,&st,sizeof(struct Student));
        char tempBuffer[100];
        sprintf(tempBuffer,"\nSuccessfully added student\nNote: Login-id is MT{Student-ID}\nStudent-Id Generated is: %d...\n~",id);
        wr(client_socket,tempBuffer,strlen(tempBuffer)+1);
        // Unlocking
        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLK,&lock);
        close(fd);
        
}

void add_teacher(int client_socket){
    
    int fd = open("records/teacher_details",O_RDWR|O_APPEND);
        if (fd == -1) {
            exit(1);
        }
        // Implementing write lock
        struct flock lock;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_END;
        lock.l_start = 0;
        lock.l_len = sizeof(struct Teacher);
        lock.l_pid = getpid();
        int f = fcntl(fd, F_SETLKW, &lock);
        if(f==-1){
            perror("Error obtaining write lock on Student Record!");
            close(fd);
        }
        int id = set_count(2);
        struct Teacher t;
        sprintf(t.username,"PROF%d",id);
        wr(client_socket,"Enter Name: ",13);
        rd(client_socket,t.name,sizeof(t.name));
        wr(client_socket,"Enter Email: ",14);
        rd(client_socket,t.email,sizeof(t.email));
        wr(client_socket,"Enter Department: ",19);
        rd(client_socket,t.department,sizeof(t.department));
        wr(client_socket,"Enter Designation: ",20);
        rd(client_socket,t.desig,sizeof(t.desig));
        wr(client_socket,"Enter Address: ",16);
        rd(client_socket,t.address,sizeof(t.address));
        t.id = id;
        strcpy(t.password,"iiitbp");
        write(fd,&t,sizeof(struct Teacher));
        char tempBuffer[100];
        sprintf(tempBuffer,"\nSuccessfully added Teacher\nNote: Login-id is PROF{Prof-ID}\nProf-Id Generated is: %d...\n~",id);
        wr(client_socket,tempBuffer,strlen(tempBuffer)+1);
        // Unlocking
        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLK,&lock);
        close(fd);
        
}

void view_participant(int client_socket, int type, int sID){
    int fd,len;
    if(type == 1){
        fd = open("records/student_details",O_RDONLY);
        if (fd == -1) {
            exit(1);
        }
        len = sizeof(struct Student);
    }
    else if(type == 2){
        fd = open("records/teacher_details",O_RDONLY);
        if (fd == -1) {
            exit(1);
        }
        len = sizeof(struct Teacher);
    }
    int id;
    char buff[10];
    if(sID == -1){
        while(1){
            wr(client_socket,"Enter the ID number to access: ",32);
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
                if(id<=0 || get_count(type)<id){
                    wr(client_socket,"Wrong id entered, Try again...~\n",33);
                }
                else{break;}
            }
        }
    }
    else id = sID;
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = (id-1)*len;
    lock.l_len = len;
    lock.l_pid = getpid();
    int f = fcntl(fd, F_SETLKW, &lock);
    if(f==-1){
        perror("Error obtaining write lock on Student Record!");
        close(fd);
    }
    lseek(fd,(id-1)*len,SEEK_SET);
    if(type==1){
        struct Student st;
        read(fd,&st,sizeof(struct Student));
        char send[100+sizeof(struct Student)];
        sprintf(send,"Name: %s\nEmail: %s\nAddress: %s\nRollno: %d\nUsername: %s~\n",st.name,st.email,st.address,st.rollNumber,st.username);
        wr(client_socket,send,strlen(send)+1);
    }
    else{
        struct Teacher t;
        read(fd,&t,sizeof(struct Teacher));
        char send[100+sizeof(struct Teacher)];
        sprintf(send,"Name: %s\nEmail: %s\nDepartment: %s\nDesignation: %s\nAddress: %s\nUsername: %s~\n",t.name,t.email,t.department,t.desig,t.address,t.username);
        wr(client_socket,send,strlen(send)+1);
    }
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
    close(fd);
}

void activate_deactivate(int client_socket, int status){
    int fd = open("records/student_details",O_RDWR);
    if (fd == -1) {
        exit(1);
    }
    int id;
    char buff[10];
    while(1){
        if(status == 1)
            wr(client_socket,"Enter the Student ID number to Activate: ",42);
        else
            wr(client_socket,"Enter the Student ID number to Deactivate: ",44);
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
            if(id<=0 || get_count(1)<id){
                wr(client_socket,"Wrong id entered, Try again...~\n",33);
            }
            else{break;}
        }
    }
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = (id-1)*sizeof(struct Student);
    lock.l_len = sizeof(struct Student);
    lock.l_pid = getpid();
    int f = fcntl(fd, F_SETLKW, &lock);
    if(f==-1){
        perror("Error obtaining write lock on Student Record!");
        close(fd);
    }
    lseek(fd,(id-1)*sizeof(struct Student),SEEK_SET);
    struct Student st;
    read(fd,&st,sizeof(struct Student));
    if(status == 1)st.isActive=true;
    else st.isActive = false;
    lseek(fd,(id-1)*sizeof(struct Student),SEEK_SET);
    write(fd,&st,sizeof(struct Student));
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
    close(fd);
}

void modify(int client_socket,int type){
    int fd,len;
    if(type == 1){
        fd = open("records/student_details",O_RDWR);
        if (fd == -1) {
            exit(1);
        }
        len = sizeof(struct Student);
    }
    else if(type == 2){
        fd = open("records/teacher_details",O_RDWR);
        if (fd == -1) {
            exit(1);
        }
        len = sizeof(struct Teacher);
    }
    int id;
    char buff[10];
    while(1){
        wr(client_socket,"Enter the ID number to access: ",32);
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
            if(id<=0 || get_count(type)<id){
                wr(client_socket,"Wrong id entered, Try again...~\n",33);
            }
            else{break;}
        }
    }
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = (id-1)*len;
    lock.l_len = len;
    lock.l_pid = getpid();
    int f = fcntl(fd, F_SETLKW, &lock);
    if(f==-1){
        perror("Error obtaining write lock on Student Record!");
        close(fd);
    }
    lseek(fd,(id-1)*len,SEEK_SET);
    if(type == 1){
        struct Student st;
        read(fd,&st,len);
        char resp[2];
        int choice;
        while(1){
            wr(client_socket,"Enter option{1. Name, 2. Email, 3. Address, 4. Age}: ",54);
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
                if(choice > 5 || choice < 1){
                    wr(client_socket,"Wrong option entered, Try again...~\n",37);
                }
                else{break;}
            }
        }
        switch (choice)
        {
        case 1:
            wr(client_socket,"Enter Name: ",13);
            rd(client_socket,st.name,sizeof(st.name));
            break;
        case 2:
            wr(client_socket,"Enter Email: ",14);
            rd(client_socket,st.email,sizeof(st.email));
            break;
        case 3: 
            wr(client_socket,"Enter Address: ",16);
            rd(client_socket,st.address,sizeof(st.address));
            break;
        case 4:
            char age[2];
            wr(client_socket,"Enter Age: ",12);
            rd(client_socket,age,sizeof(age));
            st.age = atoi(age);
            break;
        default:
            break;
        }
        lseek(fd,(id-1)*len,SEEK_SET);
        write(fd,&st,sizeof(st));
    }

     if(type == 2){
        struct Teacher st;
        read(fd,&st,len);
        char resp[2];
        int choice;
        while(1){
            wr(client_socket,"Enter option{1. Name, 2. Email, 3. Address, 4. Department, 5. Designation}: ",77);
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
                if(choice > 5 || choice < 1){
                    wr(client_socket,"Wrong option entered, Try again...~\n",37);
                }
                else{break;}
            }
        }
        switch (choice)
        {
        case 1:
            wr(client_socket,"Enter Name: ",13);
            rd(client_socket,st.name,sizeof(st.name));
            break;
        case 2:
            wr(client_socket,"Enter Email: ",14);
            rd(client_socket,st.email,sizeof(st.email));
            break;
        case 3: 
            wr(client_socket,"Enter Address: ",16);
            rd(client_socket,st.address,sizeof(st.address));
            break;
        case 4:
            wr(client_socket,"Enter Deparment: ",12);
            rd(client_socket,st.department,sizeof(st.department));
            break;
        case 5:
            wr(client_socket,"Enter Designation: ",20);
            rd(client_socket,st.desig,sizeof(st.desig));
            break;
        default:
            break;
        }

        lseek(fd,(id-1)*len,SEEK_SET);
        write(fd,&st,sizeof(st));
    }
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
    close(fd);
}

#endif