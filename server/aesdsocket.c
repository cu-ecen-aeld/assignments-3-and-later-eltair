#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

const char *OUT_FILE = "/var/tmp/aesdsocketdata";

int sock = 0;
int fd = 0;
ssize_t count = 0;
struct addrinfo *serverinfo = NULL;
FILE *f_out = NULL; // open output file
FILE *f_fd = NULL;	// open socket for buffered reading

static void err()
{
	fprintf(stderr, "%s\n", strerror(errno));
	exit(-1);
}

static void clean_up(int sig_num)
{
	syslog(LOG_INFO, "%s\n", "Caught signal, exiting");
	if (f_fd)
		fclose(f_fd);
	if (fd)
		close(fd);
	if (f_out)
		fclose(f_out);
	if (sock)
		close(sock);
	if (serverinfo)
		freeaddrinfo(serverinfo);
	remove(OUT_FILE);
	exit(0);
}

int main(int argc, char *argv[])
{
	int status;
	struct addrinfo hints;
	struct sockaddr client;
	socklen_t client_addrlen;
	const int reuse_enable = 1;

	struct sigaction new_action;
	new_action.sa_handler = clean_up;
	new_action.sa_flags = 0;
	sigemptyset(&new_action.sa_mask);
	sigaction(SIGINT, &new_action, NULL);
	sigaction(SIGTERM, &new_action, NULL);

	// initialize addr hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	// all interfaces on port 9000
	getaddrinfo("0.0.0.0", "9000", &hints, &serverinfo);

	// open the socket
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		err();

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_enable, sizeof(reuse_enable));
	setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &reuse_enable, sizeof(reuse_enable));

	// bind to the port
	if ((status = bind(sock, serverinfo->ai_addr, serverinfo->ai_addrlen)) != 0)
		err();

	// open the socket for listening
	listen(sock, 1);

	f_out = fopen(OUT_FILE, "wb"); // open output file
	while (1)
	{
		fd = accept(sock, &client, &client_addrlen);
		if (fd)
		{
			char *buf = NULL;
			size_t len = 0;
			struct sockaddr_in *addr_in = (struct sockaddr_in *)&client;
			syslog(LOG_INFO, "Accepted connection from %s\n", inet_ntoa(addr_in->sin_addr));
			f_fd = fdopen(fd, "rb"); // open socket for buffered reading

			if (f_fd)
			{
				if ((count = getline(&buf, &len, f_fd)) != -1)
				{
					printf("%s", buf);
					fputs(buf, f_out);
					fflush(f_out);
					free(buf);
					buf = NULL;

					// TODO: send file data back to client

					fclose(f_fd);
					f_fd = NULL;
					fd = 0;
				}
			}
			else
			{
				err();
			}
		}
		else
		{
			err();
		}
	}

	return 0;
}