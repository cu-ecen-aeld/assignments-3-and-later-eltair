#include "systemcalls.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    {
        int ret = system(cmd);
        if(ret != 0) return false;
    }

    return true;
}

bool my_exec(int count, char *command[], const char *outputfile) {
    pid_t pid = fork();

    if (pid > 0) { // parent
        int stat = 0;
        if(wait(&stat) == pid) {
            if(WIFEXITED(stat)) {
                int status = WEXITSTATUS(stat);
                if(status == 0) {
                    return true;
                }
            }
        }
    } else if (pid == 0) { // child
        // print debugging info
        for(int i=0; i < count; i++) {
            fprintf(stderr, "CHILD: command[%d]=%s\n", i, command[i]);
        }

        if(outputfile) {
            fprintf(stderr, "Opening output file: %s\n", outputfile);
            int out_fd = creat(outputfile, 0644);
            if(out_fd) {
                fprintf(stderr, "%s", "Redirecting standard output\n");
                dup2(out_fd, STDOUT_FILENO); // set stdout to file for redirection
                close(out_fd);
            } else {
                fprintf(stderr, "Failed to open file: %s", outputfile);
                exit(1);
            }
        }

        // run the command
        execv(command[0], command);

        // execv should not return if it's successful.
        // print error and exit if it does return
        fprintf(stderr, "CHILDERROR: %s\n", strerror(errno));
        exit(1);
    }

    return false;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    bool result = false;
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

    result = my_exec(count, command, NULL);

    va_end(args);

    return result;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    bool result = false;
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
 
    result = my_exec(count, command, outputfile);

    va_end(args);

    return result;
}
