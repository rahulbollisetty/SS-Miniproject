
// Structures to store and access various records effectively

#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_NAME_LENGTH 100
#define MAX_EMAIL_LENGTH 100
#define MAX_ADDRESS_LENGTH 100

// Administrator Details
struct AdminCredentials {
    char username[50];
    char password[50];
};

// Structure to store student details
struct Student {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    char address[MAX_ADDRESS_LENGTH];
    bool isActive;
    char gender;
    int rollNumber;
    int age;
};

// Structure to store teacher details
struct Teacher {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    char address[MAX_ADDRESS_LENGTH];
    char department[50];
    char desig[50];
    int id;
};

// Structure to store course details
struct Course{
    char cname[50];
    char department[50];
    int credits;
    int seats;
    int available;
    int profId;
    int isActive;
    int id;
};

// Structure to store enrollment details
struct Enrollment {
    int studentID;
    int courseID;
    int profID;
    int enrollmentID;
    int isEnrolled;
};

// Structure to store account count details
struct record{
int student;
int teacher;
int courses;
int enrollments;
};