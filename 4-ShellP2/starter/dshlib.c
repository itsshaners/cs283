#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <limits.h>  
#include "dshlib.h"


Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc == 1) {
            // No arguments, do nothing
            return BI_EXECUTED;
        } else if (cmd->argc == 2) {
            // Change directory
            if (chdir(cmd->argv[1]) != 0) {
                perror("cd");
                return ERR_EXEC_CMD;
            }
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);  // Print working directory correctly
                fflush(stdout);  // Ensure output is printed before next prompt
            }
            return BI_EXECUTED;
        } else {
            fprintf(stderr, "cd: too many arguments\n");
            return ERR_CMD_ARGS_BAD;
        }
    }
    return BI_NOT_BI;
}
/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
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
    cmd_buff_t cmd_buff;
    int rc;

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_line, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        // Remove the trailing newline
        cmd_line[strcspn(cmd_line, "\n")] = '\0';

        // Build command buffer
        rc = build_cmd_buff(cmd_line, &cmd_buff);
        if (rc != OK) {
            fprintf(stderr, "Error: %d\n", rc);
            continue;
        }

        // Execute built-in command
        if (exec_built_in_cmd(&cmd_buff) == BI_EXECUTED) {
            continue;
        }

        // Execute external command
        rc = exec_cmd(&cmd_buff);
        if (rc != OK) {
            fprintf(stderr, "Error: %d\n", rc);
        }

        // Free command buffer
        free_cmd_buff(&cmd_buff);
    }

    return OK;
}

int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return ERR_EXEC_CMD;
    } else if (pid == 0) {
        // Child process
        if (execvp(cmd->argv[0], cmd->argv) == -1) {
            switch (errno) {
                case ENOENT:
                    fprintf(stderr, "Command not found: %s\n", cmd->argv[0]);
                    break;
                case EACCES:
                    fprintf(stderr, "Permission denied: %s\n", cmd->argv[0]);
                    break;
                default:
                    perror("execvp");
            }
            exit(errno);
        }
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            if (exit_status != 0) {
                fprintf(stderr, "Command exited with status: %d\n", exit_status);
            }
        }
    }
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    cmd_buff->argc = 0;
    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));

    char buffer[SH_CMD_MAX];  // Temporary buffer
    int buf_index = 0;
    bool in_quotes = false;
    char *ptr = cmd_line;

    while (*ptr) {
        while (*ptr == ' ' && !in_quotes) ptr++;  // Skip spaces outside quotes

        if (*ptr == '"') {
            in_quotes = !in_quotes;  // Toggle quote state
            ptr++;
        }

        char *token_start = buffer + buf_index;  // Start of argument

        while (*ptr && (in_quotes || (*ptr != ' '))) {
            if (*ptr == '"') {
                in_quotes = !in_quotes;
            } else {
                buffer[buf_index++] = *ptr;
            }
            ptr++;
        }

        buffer[buf_index++] = '\0';  // Null-terminate argument

        if (cmd_buff->argc >= CMD_ARGV_MAX - 1) return ERR_CMD_OR_ARGS_TOO_BIG;

        cmd_buff->argv[cmd_buff->argc] = strdup(token_start);
        if (!cmd_buff->argv[cmd_buff->argc]) return ERR_MEMORY;

        cmd_buff->argc++;
    }

    cmd_buff->argv[cmd_buff->argc] = NULL;
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    for (int i = 0; i < cmd_buff->argc; i++) {
        free(cmd_buff->argv[i]);
    }
    cmd_buff->argc = 0;
    return OK;
}
