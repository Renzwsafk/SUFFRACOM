#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#ifdef __linux__

#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <poll.h>

#endif

#include "server.h"
#include "include/utils.h"


static void net_err(const char *fmt, ...)
{
	int errno_save;
	va_list ap;

	errno_save = errno;

	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	fprintf(stdout, "\n");
	fflush(stdout);

	if (errno_save != 0)
	{
		fprintf(stdout, "(errno = %d) : %s\n", errno_save, strerror(errno_save));
		fprintf(stdout, "\n");
		fflush(stdout);
	}

	va_end(ap);

	exit(EXIT_FAILURE);
}

static char *bin2hex(const unsigned char *input, size_t len)
{
	char *res;
	char *hexits = "0123456789ABCDEF";

	if (input == NULL || len <= 0) return NULL;

	int result_length = (len*3)+1;

	res = malloc(result_length);
	bzero(res, result_length);

	for (int i = 0; i < len; i++)
	{
		res[i*3] = hexits[input[i] >> 4];
		res[(i*3)+1] = hexits[input[i]  & 0x0F];
		res[(i*3)+2] = ' ';
	}

	return res;
}


void server_init(server_t *server)
{

	if ((server->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) net_err("Socket error: failed to create socket");

	bzero(&server->servaddr, sizeof(server->servaddr));
	server->servaddr.sin_family = AF_INET;
	server->servaddr.sin_port = htons(SERVER_PORT); 

}

void server_connect(server_t* server)
{
	if ((bind(server->socket, (struct sockaddr *)&server->servaddr, sizeof(server->servaddr))) < 0) net_err("Socket error: binding address failed");
	if ((listen(server->socket, 10) < 0)) net_err("Socket error:"); 

	server->client = accept(server->socket, 0, 0);
}

void server_send(server_t *server)
{
	struct pollfd fds[2] = {
		{
			0,
			POLLIN,
			0
		},
		{
			server->client,
			POLLIN,
			0
		}
	};

	poll(fds, 2, 50000);

	if (fds[0].revents & POLLIN)
	{
		read(0, server->buf, MAXLINE+1); 
		send(server->client, server->buf, MAXLINE+1, 0);
	}
	    else if (fds[1].revents & POLLIN)
	    {
			if ((recv(server->client, server->buf, MAXLINE+1, 0)) < 0) net_err("recv failed");
			printf("%s\n", server->buf);
	    }
}
