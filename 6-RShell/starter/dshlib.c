#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, DSH_ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int exec_local_cmd_loop() {
    char cmd_line[SH_CMD_MAX];
    command_list_t clist;
    int rc;
    int last_status = 0;  // Track last command status

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_line, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        cmd_line[strcspn(cmd_line, "\n")] = '\0';
        if (strcmp(cmd_line, EXIT_CMD) == 0) {
            printf("dsh4> exiting...\n");
            break;
        }
        rc = build_cmd_list(cmd_line, &clist);
        if (rc != OK) {
            if (rc == WARN_NO_CMDS) {
                printf("%s\n", CMD_WARN_NO_CMD);
            } else if (rc == ERR_TOO_MANY_COMMANDS) {
                printf("dsh4> %s\n", CMD_ERR_PIPE_LIMIT);  // Ensure the error message matches
                return rc;  // Ensure we exit with the right error code
            } else {
                fprintf(stderr, "Error: %d\n", rc);
            }
            continue;
        }

        // Check for built-in commands first
        if (exec_built_in_cmd(&clist.commands[0], &last_status) == BI_EXECUTED) {
            continue;
        }

        rc = execute_pipeline(&clist);
        if (rc != OK) {
            printf("dsh4> %s\n", CMD_ERR_EXECUTE);  // Ensure the error message matches
        }
        last_status = rc;  // Update last status
    }
    return last_status;
}

void close_all_pipes(int pipes[][2], int num_pipes) {
    for (int i = 0; i < num_pipes; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

void setup_pipes(int i, int num_cmds, int pipes[][2]) {
    if (i > 0) { // Read from previous pipe
        dup2(pipes[i - 1][0], STDIN_FILENO);
    }
    if (i < num_cmds - 1) { // Write to next pipe
        dup2(pipes[i][1], STDOUT_FILENO);
    }
    close_all_pipes(pipes, num_cmds - 1);
}

void parse_command_args(cmd_buff_t *cmd, char *args[]) {
    args[0] = cmd->argv[0];
    for (int i = 1; i < cmd->argc; i++) {
        args[i] = cmd->argv[i];
    }
    args[cmd->argc] = NULL;
}

void handle_redirection(cmd_buff_t *cmd) {
    for (int i = 0; i < cmd->argc; i++) {
        if (strcmp(cmd->argv[i], "<") == 0) {
            int fd = open(cmd->argv[i + 1], O_RDONLY);
            if (fd == -1) {
                perror("open");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
            cmd->argv[i] = NULL;
            cmd->argc = i;  // Update argc to exclude redirection part
            break;
        } else if (strcmp(cmd->argv[i], ">") == 0) {
            int fd = open(cmd->argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("open");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            cmd->argv[i] = NULL;
            cmd->argc = i;  // Update argc to exclude redirection part
            break;
        } else if (strcmp(cmd->argv[i], ">>") == 0) {
            int fd = open(cmd->argv[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1) {
                perror("open");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            cmd->argv[i] = NULL;
            cmd->argc = i;  // Update argc to exclude redirection part
            break;
        }
    }
}

int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return ERR_EXEC_CMD;
    }
    if (pid == 0) { // Child process
        char *args[CMD_ARGV_MAX];
        parse_command_args(cmd, args);
        handle_redirection(cmd);
        execvp(args[0], args);
        perror("execvp");  // Print error if execvp fails
        exit(127);         // Exit with standard "command not found" error code
    }
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);  // Return child's exit code
    }
    return ERR_EXEC_CMD;
}

int execute_pipeline(command_list_t *clist) {
    int num_cmds = clist->num;
    int pipes[CMD_MAX - 1][2];
    pid_t pids[CMD_MAX];
    int last_status = OK;  // Store the last command’s exit status

    // Create pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return ERR_EXEC_CMD;
        }
    }

    for (int i = 0; i < num_cmds; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            close_all_pipes(pipes, num_cmds - 1);
            return ERR_EXEC_CMD;
        }
        if (pids[i] == 0) { // Child process
            setup_pipes(i, num_cmds, pipes);
            char *args[CMD_ARGV_MAX];
            parse_command_args(&clist->commands[i], args);
            handle_redirection(&clist->commands[i]);
            execvp(args[0], args);
            perror("execvp");  // Print error if execvp fails
            close_all_pipes(pipes, num_cmds - 1);
            exit(127);  // Standard exit code for "command not found"
        }
    }

    close_all_pipes(pipes, num_cmds - 1);
    
    // Wait for all processes and capture the last command’s exit status
    for (int i = 0; i < num_cmds; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status)) {
            last_status = WEXITSTATUS(status);  // Capture the exit status of the last command
        }
    }

    return last_status;  // Return the exit status of the last command
}

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    memset(clist, 0, sizeof(command_list_t));
    clist->num = 0;

    char *saveptr;
    char *token = strtok_r(cmd_line, PIPE_STRING, &saveptr);

    while (token != NULL) {
        if (clist->num + 1 > CMD_MAX) {  // Fix: Properly check limit
            printf("%s\n", CMD_ERR_PIPE_LIMIT);  // Print error message
            return ERR_TOO_MANY_COMMANDS;  // Return correct error code
        }
        
        cmd_buff_t *cmd = &clist->commands[clist->num];
        build_cmd_buff(token, cmd);
        clist->num++;
        token = strtok_r(NULL, PIPE_STRING, &saveptr);
    }

    return (clist->num > 0) ? OK : WARN_NO_CMDS;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    char *arg;
    int argc = 0;

    // Initialize the command buffer
    memset(cmd_buff, 0, sizeof(cmd_buff_t));

    // Tokenize the command string into arguments
    while ((arg = strtok_r(cmd_line, " ", &cmd_line)) != NULL) {
        if (argc >= CMD_ARGV_MAX) {
            fprintf(stderr, "Error: Too many arguments\n");
            return -1;  // Return an error code
        }
        cmd_buff->argv[argc++] = arg;
    }
    cmd_buff->argc = argc;
    return 0;  // Return success code
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd, int *last_status) {
    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc == 1) {
            // No arguments, do nothing
            return BI_EXECUTED;
        } else if (cmd->argc == 2) {
            // Change directory
            if (chdir(cmd->argv[1]) != 0) {
                perror("cd");
                *last_status = 1;  // Set error status
                return BI_EXECUTED;
            }
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);  // Print working directory correctly
                fflush(stdout);  // Ensure output is printed before next prompt
            }
            *last_status = 0;  // Set success status
            return BI_EXECUTED;
        } else {
            fprintf(stderr, "cd: too many arguments\n");
            *last_status = 1;  // Set error status
            return BI_EXECUTED;
        }
    }

    if (strcmp(cmd->argv[0], "exit") == 0) {
        printf("dsh4> exiting...\n");
        exit(*last_status);  // Exit with last command's status
    }

    if (strcmp(cmd->argv[0], "echo") == 0 && cmd->argc == 2 && strcmp(cmd->argv[1], "$?") == 0) {
        printf("%d\n", *last_status);
        *last_status = 0;  // Set success status
        return BI_EXECUTED;
    }

    return BI_NOT_BI;  // Not a built-in command
}
