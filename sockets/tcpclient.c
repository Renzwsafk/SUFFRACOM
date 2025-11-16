#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
	#include <sys/socket.h>	
	#include <sys/types.h>
	#include <sys/time.h>
	#include <unistd.h>
	#include <signal.h>
	#include <arpa/inet.h>
	#include <stdarg.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <sys/ioctl.h>
	#include <netdb.h>

#elif __APPLE__
	#include <sys/socket.h>	
	#include <sys/types.h>
	#include <sys/time.h>
	#include <unistd.h>
	#include <signal.h>
	#include <arpa/inet.h>
	#include <stdarg.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <sys/ioctl.h>
	#include <netdb.h>
#elif _WIN64
	#include <windows.h>
#endif


#define SERVER_PORT 80
#define MAXLINE 4096


int main(int, char**);
static void net_error(const char*, ...);



void tcpclient_main(int argc, char **argv, socket_t *sock_fd)
{
	char send_line[MAXLINE], recv_line[MAXLINE];

	if (argc != 2) net_error("usage: %s <server address>", argv[0]);
	if ((sock_fd.sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) net_error("Error: failed to create socket");
	
	bzero(&sock_fd.servaddr, sizeof(sock_fd.servaddr));
	sock_fd.servaddr.sin_family = AF_INET;
	sock_fd.servaddr.sin_port = htons(SERVER_PORT);

	

	exit(EXIT_SUCCESS);
}

void tcpclient_connect_server(socket_t *sock)
{
	if (inet_pton(AF_INET, argv[1], &sock->servaddr.sin_addr) <= 0) net_error("inet_pton error: %s ", argv[1]);
	if (connect(sock->fd, (struct sockaddr *)&sock->servaddr, sizeof(sock->servaddr)) < 0) net_error("Connection failed");
}

void tcpclient_write_server(socket_t *sock, char *str)
{
	char *send_line, *send_bytes;
	sprintf(send_line, str);
	send_bytes = strlen(send_line);

	if (write(sock->fd, send_line, send_bytes) != send_bytes) net_error("write error");
}

void tcpclinet_read_server(socket_t *sock)
{
	memset(recv_line, 0, MAXLINE);

	while ((n = read(sock_fd, recv_line, MAXLINE-1)) > 0) printf("%s", recv_line);
	if (n < 0) net_error("read error");
}

static void net_error(const char *format, ...)
{
	int errno_save;
	va_list ap;

	errno_save = errno;

	va_start(ap, format);
	vfprintf(stdout, format, ap);
	fprintf(stdout, "\n");
	fflush(stdout);

	if (errno_save != 0)
	{
		fprintf(stdout, "(errno = %d) : %s\n", errno_save, strerror(errno_save));
		fprintf(stdout, "\n");
		fflush(stdout);
	}

	va_end(ap);

	exit(1);
}

