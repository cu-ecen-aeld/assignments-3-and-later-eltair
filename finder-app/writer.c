#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char *writefile = argv[1];
    char *writestr = argv[2];
    int len = 0;
    int fileDescriptor;
    ssize_t bytesWritten;

    openlog("writer", 0, LOG_USER);

    // check for parameter count (2 params + program name) and print usage
    if(argc != 3) {
        printf("Usage: ./writer <writefile> <writestr>\n");
        return 1;
    }

    // open the file for writing
    fileDescriptor = creat(writefile, 0644);
    if (fileDescriptor == -1) {
        syslog(LOG_ERR, "Error creating file: %s\n", writefile);
        return 1;
    }

    // determine how many bytes to write
    len = strlen(writestr);

    // loop until all bytes are written to the file
    while (len > 0) {
        bytesWritten = write(fileDescriptor, writestr, len); // write to file
        if(bytesWritten == -1) {
            syslog(LOG_ERR, "%s", "Error writing string\n");
            return 1;
        }
        len -= bytesWritten; // keep track of how many bytes are left
    }
}