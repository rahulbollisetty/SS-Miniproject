
// Helper function to handle login activity for Admin, Faculty and student

#ifndef LOGIN_H
#define LOGIN_H
#define MAX_MESSAGE_SIZE 4096

int login(int client_socket){
    char buffer[MAX_MESSAGE_SIZE];
    wr(client_socket,LOGIN_MENU_STR,LOGIN_MENU_LEN);
    rd(client_socket, buffer,2);
    int res = atoi(buffer);
    if(res<1 || res > 3){
        wr(client_socket,"Wrong option entered\n#",23);
        return 0;
    }
    char username[50], password[50];
    wr(client_socket,"Username: ",11);
    rd(client_socket, username,sizeof(username));
    wr(client_socket,"Password: !",12);
    rd(client_socket, password,sizeof(password));
    switch (res)
    {
        
    case 1:
        // Admin
        if(admin_login(client_socket,username,password)){
            handle_Admin(client_socket);
            return 1;
        }
        else{
            return 0;
        }
        
        break;
    case 2:
        // Teacher
        int profSession;
        if(faculty_login(client_socket,username,password, &profSession)){
            handle_faculty(client_socket,profSession);
            return 1;
        }
        else{
            return 0;
        }
        break;
    case 3:
        // Student
        int session;
        if(student_login(client_socket,username,password, &session)){
            handle_student(client_socket,session);
            return 1;
        }
        else{
            return 0;
        }
        break;
    default:
        break;
    }
}

#endif