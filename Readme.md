# CS 513 Software Systems Mini Project - ERP

The project aims to develop a Academia Portal that is user-friendly and multifunctional.(Reference: [link](https://iiitbac-my.sharepoint.com/:b:/g/personal/b_rahul_iiitb_ac_in/ETglz11YA0pOralqfTEICbYBPWfR_KKmJCHLAg1vpYwhRQ?e=eVe88O)). The project mainly deals with concurrent course management operations in a safe and secure environment.

### Features of ERP:
1. Handling multiple connections simultaneously.
2. Socket programming is used to implement the client-server model.
3. TCP connection for effective and reliable communication.
4. Storage/Database: Flat file system is used.
5. Concurrency: File Locking is used.
7. System calls related to process management, file management, file locking and inter-process communication mechanisms is used.
8. Password-protected login system for admin, faculty and student.


### Functionalities
#### Admin Module:
After successful login of admin, can perform the following operations:
1. Add Student
2. View Student details
3. Add Teacher
4. View Teacher details
5. Activae Student
6. Block Student
7. Modify Student Details
8. Modify Teacher Details
9. Exit

#### Faculty Module:
After successful login of faculty, can perform the following operations:
1. View Offering Courses
2. Add New course
3. Remove Course from Catalog
4. Modify Course Details
5. View Course Enrollments
6. Exit

#### Student Module:
After successful login of student, can perform the following operations:
1. View All Offering Courses
2. Enroll New course
3. Drop Course
4. View Course Enrollments
5. Exit

## Setup and Execution
First clone the repository 

    enter code here

> The client-server socket uses 16000 port number
> Make sure that no other processs is using that number

Run the server by running the following on your command line:

    ./server
Run the client by running the following on your command line:

    ./client

> The server can handle multiple clients, so you can execute client code simultaneously on different terminals

- Admin default credentials:
  - Username: `root`
  - Password: `root`

B Rahul
MTech CSE [MT2023038]
International Institute of Information Technology, Bangalore.


