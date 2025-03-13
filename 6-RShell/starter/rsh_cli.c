#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <fcntl.h>

#include "dshlib.h"
#include "rshlib.h"

/*
 * exec_remote_cmd_loop(server_ip, port)
 *      server_ip:  a string in ip address format, indicating the servers IP
 *                  address.  Note 127.0.0.1 is the default meaning the server
 *                  is running on the same machine as the client
 *              
 *      port:   The port the server will use.  Note the constant 
 *              RDSH_DEF_PORT which is 1234 in rshlib.h.  If you are using
 *              tux you may need to change this to your own default, or even
 *              better use the command line override -c implemented in dsh_cli.c
 *              For example ./dsh -c 10.50.241.18:5678 where 5678 is the new port
 *              number and the server address is 10.50.241.18    
 * 
 *      This function basically implements the network version of 
 *      exec_local_cmd_loop() from the last assignemnt.  It will:
 *  
 *          1. Allocate buffers for sending and receiving data over the 
 *             network
 *          2. Create a network connection to the server, getting an active
 *             socket descriptor
 *          3. Loop to accept commands from the user, sending them to the
 *             server and then receiving the response from the server. 
 *          4. When the user enters the `exit` command, the loop terminates
 *             and the function returns. 
 * 
 *  Returns:
 * 
 *      OK:  The command loop executed successfully. 
 * 
 *      ERR_RDSH_COMMUNICATION:  The send() or recv() socket call returned an error.
 * 
 */
int exec_remote_cmd_loop(char *address, int port) {
    int cli_socket;
    struct sockaddr_in server_addr;
    char *cmd_buff;
    char *rsp_buff;
    int rc;

    cmd_buff = malloc(RDSH_COMM_BUFF_SZ);
    rsp_buff = malloc(RDSH_COMM_BUFF_SZ);
    if (cmd_buff == NULL || rsp_buff == NULL) {
        return ERR_RDSH_CLIENT;
    }

    cli_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (cli_socket < 0) {
        perror("socket");
        return ERR_RDSH_COMMUNICATION;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, address, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        return ERR_RDSH_COMMUNICATION;
    }

    if (connect(cli_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        return ERR_RDSH_COMMUNICATION;
    }

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, RDSH_COMM_BUFF_SZ, stdin) == NULL) {
            printf("\n");
            break;
        }
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        if (send(cli_socket, cmd_buff, strlen(cmd_buff) + 1, 0) < 0) {
            perror("send");
            rc = ERR_RDSH_COMMUNICATION;
            break;
        }

        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            break;
        }

        int rsp_size;
        while ((rsp_size = recv(cli_socket, rsp_buff, RDSH_COMM_BUFF_SZ, 0)) > 0) {
            rsp_buff[rsp_size] = '\0';
            printf("%s", rsp_buff);
            if (rsp_buff[rsp_size - 1] == RDSH_EOF_CHAR) {
                break;
            }
        }

        if (rsp_size < 0) {
            perror("recv");
            rc = ERR_RDSH_COMMUNICATION;
            break;
        }
    }

    free(cmd_buff);
    free(rsp_buff);
    close(cli_socket);
    return OK;
}

/*
 * start_client(server_ip, port)
 *      server_ip:  a string in ip address format, indicating the servers IP
 *                  address.  Note 127.0.0.1 is the default meaning the server
 *                  is running on the same machine as the client
 *              
 *      port:   The port the server will use.  Note the constant 
 *              RDSH_DEF_PORT which is 1234 in rshlib.h.  If you are using
 *              tux you may need to change this to your own default, or even
 *              better use the command line override -c implemented in dsh_cli.c
 *              For example ./dsh -c 10.50.241.18:5678 where 5678 is the new port
 *              number and the server address is 10.50.241.18    
 * 
 *      This function basically starts the client by calling exec_remote_cmd_loop()
 *      with the provided server IP and port.
 * 
 *  Returns:
 * 
 *      OK:  The client started successfully. 
 * 
 *      ERR_RDSH_COMMUNICATION:  The send() or recv() socket call returned an error.
 * 
 */
int start_client(char *server_ip, int port) {
    return exec_remote_cmd_loop(server_ip, port);
}

/*
 * client_cleanup(cli_socket, cmd_buff, rsp_buff, rc)
 *      cli_socket:  The client-side socket that is connected to the server
 *      cmd_buff:    The command buffer used for sending commands to the server
 *      rsp_buff:    The response buffer used for receiving responses from the server
 *      rc:          The return code to return from the function
 * 
 *  Cleans up resources used by the client.
 * 
 *  Returns:
 * 
 *      The provided return code (rc).
 * 
 */
int client_cleanup(int cli_socket, char *cmd_buff, char *rsp_buff, int rc) {
    if (cli_socket >= 0) {
        close(cli_socket);
    }
    if (cmd_buff != NULL) {
        free(cmd_buff);
    }
    if (rsp_buff != NULL) {
        free(rsp_buff);
    }
    return rc;
}