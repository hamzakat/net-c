#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>

#define SERVERPORT 5555
#define BUFFER_SIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 1

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void handle_connection(int client_sock);
int check(int exp, const char *msg);    // error handler

int main(int argc, char const *argv[])
{
    int server_sock, client_sock, addr_size;
    SA_IN server_addr, client_addr;

    // create socket (SOCK_STREAM make TCP connection)
    check(server_sock = socket(AF_INET, SOCK_STREAM, 0),
        "Failed to create socket!"
    );

    // init address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVERPORT);

    check(bind(server_sock, (SA*)&server_addr, sizeof(server_addr)),
        "Binding failed!"
    );

    check(listen(server_sock, SERVER_BACKLOG),
        "Listening failed!"
    );

    while (true) {
        printf("Waiting for connection \n");

        addr_size = sizeof(SA_IN);
        
        // handle the client socket
        check(client_sock = 
            accept(server_sock, (SA*)&client_addr, (socklen_t*)&addr_size),
            "Accept failed"
        );
        printf("Connected \n");

        handle_connection(client_sock);
    }
    
    
    return 0;
}

void handle_connection(int client_sock) 
{
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    int msg_size = 0;
    char actual_path[PATH_MAX + 1];

    // read the client's meassage (file name)
    while((bytes_read = read(client_sock, buffer + msg_size, sizeof(buffer) - msg_size - 1)) > 0) {
        msg_size += bytes_read;
        if(msg_size > BUFFER_SIZE - 1 || buffer[msg_size - 1] == '\n') {
            break;
        }
    }

    check(bytes_read, "Receiving error!");

    // end the message with 0 instead of "\n"
    buffer[msg_size - 1] = 0;
    
    // print out the requset payload
    printf("Request: %s \n", buffer);
    fflush(stdout);

    // check the path validity
    if(realpath(buffer, actual_path) == NULL) {
        printf("Bad path error: %s \n", buffer);
        close(client_sock);
        return;
    }

    // read file and send the content to the client
    FILE *f = fopen(actual_path, "r");
    if(f == NULL) {
        printf("Open file error: %s\n", buffer);
        close(client_sock);
        return;
    }

    /** 
     * Warning: This is an insecure code. There must be limitations on the files allowed in production programs.
     */
    while((bytes_read = fread(buffer, 1, BUFFER_SIZE, f)) > 0) {
        printf("Sending %zu bytes ...\n", bytes_read);
        
        // write buffer to the socket
        write(client_sock, buffer, bytes_read);
    }

    close(client_sock);
    fclose(f);
    printf("Closing connection. \n");
}

int check(int exp, const char *msg)
{
    if(exp == SOCKETERROR) {
        perror(msg);
        exit(1);
    }
    return 1;
}