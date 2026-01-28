#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "lcall.h"

/* ------------------------------------------------------------------------- */
#define SIN4_ADDR(sa)   ((struct sockaddr_in *)sa)->sin_addr
#define SIN4_PORT(sa)   ((struct sockaddr_in *)sa)->sin_port
#define SIN6_ADDR(sa)   ((struct sockaddr_in6 *)sa)->sin6_addr
#define SIN6_PORT(sa)   ((struct sockaddr_in6 *)sa)->sin6_port

/* ------------------------------------------------------------------------- */
int fn_getaddrinfo(void **args) {

	const char *node = args[0];
	const char *service = args[1];
	struct addrinfo hints, *r = NULL, *res = NULL;
	char buf[INET6_ADDRSTRLEN];
	const char *lbr = "";
	const char *rbr = "";
	int port = -1;
	int af;
	void *addr;
	socklen_t buflen;


	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_ADDRCONFIG;

	if (getaddrinfo(node, service ? service : NULL, &hints, &res)) {
		perror("getaddrinfo");
		return 1;
	}

	r = res;
	while(r) {
		memset(&buf, 0, INET6_ADDRSTRLEN);

		switch (r->ai_family) {
			case AF_INET:
				af = AF_INET;
				addr = &SIN4_ADDR(r->ai_addr);
				buflen = INET_ADDRSTRLEN;
				lbr = "";
				rbr = "";
				port = ntohs(SIN4_PORT(r->ai_addr));
			break;

			case AF_INET6:
				af = AF_INET6;
				addr = &SIN6_ADDR(r->ai_addr);
				buflen = INET6_ADDRSTRLEN;
				lbr = "[";
				rbr = "]";
				port = ntohs(SIN6_PORT(r->ai_addr));
			break;

			default:
				fprintf(stderr, "Unknown address family %d\n", r->ai_family);
			return 1;
		}

		if (!inet_ntop(af, addr, buf, buflen)) {
			perror("inet_ntop");
			return 1;
		}

		if (service)
			printf("%s%s%s:%d\n", lbr, buf, rbr, port);
		else
			printf("%s%s%s\n", lbr, buf, rbr);

		r = r->ai_next;
	}

	freeaddrinfo(res);
	return 0;
}

/* ------------------------------------------------------------------------- */
int fn_getnameinfo(void **args) {
	const char *node    = args[0];
	const char *service = args[1];

	printf("getnameinfo(%s, %s)\n", node, service);

	return 0;
}
