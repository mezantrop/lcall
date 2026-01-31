/*
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2026 Mikhail Zakharov
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <net/if.h>

#include "lcall.h"
#include "funcs.h"


/* ------------------------------------------------------------------------- */
int fn_getaddrinfo(void **args) {
	const char *node = args[0];
	const char *service = args[1];
	const char *str_af = args[2];
	struct addrinfo hints, *r = NULL, *res = NULL;
	char buf[INET6_ADDRSTRLEN];
	const char *lbr = "";
	const char *rbr = "";
	int port = -1;
	int af;
	void *addr;
	socklen_t buflen;

	memset(&hints, 0, sizeof(struct addrinfo));
	if (!str_af) {
		hints.ai_family = AF_UNSPEC;
	} else if (!strcmp(str_af, "4")) {
		hints.ai_family = AF_INET;
	} else if (!strcmp(str_af, "6")) {
		hints.ai_family = AF_INET6;
	} else {
		fprintf(stderr, "Invalid address family: %s (use 4 or 6)\n", str_af);
		return 1;
	}

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
				addr = &((struct sockaddr_in *)r->ai_addr)->sin_addr;
				buflen = INET_ADDRSTRLEN;
				lbr = "";
				rbr = "";
				port = ntohs(((struct sockaddr_in *)r->ai_addr)->sin_port);
			break;

			case AF_INET6:
				af = AF_INET6;
				addr = &((struct sockaddr_in6 *)r->ai_addr)->sin6_addr;
				buflen = INET6_ADDRSTRLEN;
				lbr = "[";
				rbr = "]";
				port = ntohs(((struct sockaddr_in6 *)r->ai_addr)->sin6_port);
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
	const char *addr = args[0];
	const char *service = args[1];

	struct sockaddr_storage ss;
	socklen_t salen;
	char host[NI_MAXHOST];
	char serv[NI_MAXSERV];

	char *end;
	long port = strtol(service, &end, 10);
	if (*end != '\0' || port < 0 || port > 65535) {
		fprintf(stderr, "Invalid service number: %s\n", service);
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
			fprintf(stderr, "Invalid IP address: %s\n", addr);
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

/* ------------------------------------------------------------------------- */
int fn_if_nametoindex(void **args) {
	unsigned int i;


	if (!(i = if_nametoindex(args[0]))) {
		perror("if_nametoindex");
		return 1;
	}

	printf("%d\n", i);
	return 0;
}

/* ------------------------------------------------------------------------- */
int fn_if_indextoname(void **args) {
	char ifname[IF_NAMESIZE];
	const char *iidx = args[0];

	char *end;
	long ifindex = strtol(iidx, &end, 10);
	if (*end != '\0' || ifindex < 1 || ifindex > UINT_MAX) {
		fprintf(stderr, "Invalid interface index: %s\n", iidx);
		return 1;
	}

	if (!if_indextoname((unsigned int)ifindex, ifname)) {
		perror("if_indextoname");
		return 1;
	}

	printf("%s\n", ifname);
	return 0;
}
