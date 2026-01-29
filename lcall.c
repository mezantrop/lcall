#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lcall.h"
#include "funcs.h"


/* ------------------------------------------------------------------------- */
struct arg_desc getaddrinfo_args[] = {
	{ "node",		1 },
	{ "service",	0 },

};

struct arg_desc getnameinfo_args[] = {
	{ "addr",		1 },
	{ "service",	0 },
};

struct arg_desc if_nametoindex_args[] = {
	{ "ifname",		1 },
};

struct arg_desc if_indextoname_args[] = {
	{ "ifindex",	1 },
};

struct arg_desc getpriority_args[] = {
	{ "pid",		1 },
};

struct func_desc funcs[] = {
	{
		.name = "getaddrinfo",
		.fn   = fn_getaddrinfo,
		.argc = NARGS(getaddrinfo_args),
		.argv = getaddrinfo_args,
	},
	{
		.name = "getnameinfo",
		.fn   = fn_getnameinfo,
		.argc = NARGS(getnameinfo_args),
		.argv = getnameinfo_args,
	},
	{
		.name = "if_nametoindex",
		.fn   = fn_if_nametoindex,
		.argc = NARGS(if_nametoindex_args),
		.argv = if_nametoindex_args,
	},
	{
		.name = "if_indextoname",
		.fn   = fn_if_indextoname,
		.argc = NARGS(if_indextoname_args),
		.argv = if_indextoname_args,
	},
	{
		.name = "getpriority",
		.fn   = fn_getpriority,
		.argc = NARGS(getpriority_args),
		.argv = getpriority_args,
	},
};


/* ------------------------------------------------------------------------- */
struct func_desc *find_func(const char *name) {
	for (size_t i = 0; i < NARGS(funcs); i++) {
		if (!strcmp(funcs[i].name, name))
			return &funcs[i];
	}

	return NULL;
}

/* ------------------------------------------------------------------------- */
char *get_kv_value(const char *arg) {
	char *eq = strchr(arg, '=');
	if (!eq || !*(eq + 1)) return NULL;

	return eq + 1;
}

/* ------------------------------------------------------------------------- */
int build_args(struct func_desc *f, int argc, char **argv, void **out_args) {
	for (int i = 0; i < f->argc; i++) {
		out_args[i] = NULL;
		for (int j = 0; j < argc; j++) {
			if (!strncmp(argv[j], f->argv[i].name,
				strlen(f->argv[i].name)) &&
				argv[j][strlen(f->argv[i].name)] == '=') {

				out_args[i] = get_kv_value(argv[j]);
				break;
			}
		}

		if (f->argv[i].mandatory && !out_args[i]) {
			fprintf(stderr, "Fatal! Missing argument: %s\n",
				f->argv[i].name);

			return -1;
		}

	}
	return 0;
}

/* ------------------------------------------------------------------------- */
int main(int argc, char* argv[]) {

	if (argc < 2) {
		fprintf(stderr,
			"Usage: lcall <func> [k=v ...]\n"
			"\n"
			"Examples:\n"
			"  lcall getaddrinfo node=<hostname> [service=<port>]   : resolve host/service\n"
			"  lcall getnameinfo addr=<IP> [service=<port>]         : socket address -> host/service\n"
			"  lcall if_nametoindex name=<ifname>                   : interface name -> index\n"
			"  lcall if_indextoname index=<ifindex>                 : index -> interface name\n"
			"  lcall setpriority pid=<pid> prio=<value>             : set process priority\n"
			"  lcall getpriority pid=<pid>                          : get process priority\n"
		);
		return 1;
	}

	struct func_desc *f = find_func(argv[1]);
	if (!f) {
		fprintf(stderr, "Fatal! Unknown function: %s\n", argv[1]);
		return 1;
	}

	void *args[f->argc];

	if (build_args(f, argc - 2, argv + 2, args) < 0)
		return 1;

	return f->fn(args);

    return 0;
}