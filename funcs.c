#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
	const char *addr    = args[0];
	const char *service = args[1];

	struct sockaddr_storage ss;
	socklen_t salen;
	char host[NI_MAXHOST];
	char serv[NI_MAXSERV];

	char *end;
	long port = strtol(service, &end, 10);
	if (*end != '\0' || port < 0 || port > 65535) {
		fprintf(stderr, "Wrong service number: %s\n", service);
		return 1;
	}

	if (inet_pton(AF_INET, addr,
		&((struct sockaddr_in *)&ss)->sin_addr) == 1) {

		struct sockaddr_in *sin = (struct sockaddr_in *)&ss;
		sin->sin_family = AF_INET;
		sin->sin_port = htons(port);
		salen = sizeof(*sin);
	} else
		if (inet_pton(AF_INET6, addr,
			&((struct sockaddr_in6 *)&ss)->sin6_addr) == 1) {

			struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&ss;
			sin6->sin6_family = AF_INET6;
			sin6->sin6_port = htons(port);
			salen = sizeof(*sin6);
		} else {
			fprintf(stderr, "invalid IP address: %s\n", addr);
			return -1;
		}

	if (getnameinfo((struct sockaddr *)&ss, salen, host, sizeof(host),
		serv, sizeof(serv), NI_NAMEREQD | NI_NUMERICSERV)) {

		perror("getnameinfo");
		return 1;
	}

	printf("%s:%s\n", host, serv);

	return 0;
}
