
// Menu for Admin, Faculty and Student

#ifndef MENU_STRING_H
#define MENU_STRING_H

const char *LOGIN_MENU_STR =
    "..........................Welcome to Academia..........................\n"
    "Login type:\n"
    "1. Admin\n"
    "2. Teacher\n"
    "3. Student\n"
    "Enter choice: "
    ;
const int LOGIN_MENU_LEN = 130;

const char *ADMIN_MENU_STR = 
    "..........................Welcome to Admin Menu..........................\n"
    "1. Add Student\n"
    "2. View Student details\n"
    "3. Add Teacher\n"
    "4. View Teacher details\n"
    "5. Activae Student\n"
    "6. Block Student\n"
    "7. Modify Student Details\n"
    "8. Modify Teacher Details\n"
    "9. Exit\n"
    "Enter Choice\n"
;
const int ADMIN_MENU_LEN = 262;

const char *TEACHER_MENU_STR = 
    "..........................Welcome to Faculty Menu..........................\n"
    "1. View Offering Courses\n"
    "2. Add New course\n"
    "3. Remove Course from Catalog\n"
    "4. Modify Course Details\n"
    "5. View Course Enrollments\n"
    "6. Exit\n"
    "Enter Choice: \n"
;
const int TEACHER_MENU_LEN = 225;

const char *STUDENT_MENU_STR = 
    "..........................Welcome to Student Menu..........................\n"
    "1. View All Offering Courses\n"
    "2. Enroll New course\n"
    "3. Drop Course\n"
    "4. View Course Enrollments\n"
    "5. Exit\n"
    "Enter Choice: \n"
;
const int STUDENT_MENU_LEN = 192;
#endif