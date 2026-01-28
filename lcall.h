/* Argumet data types */
enum arg_type {
	ARG_INT,
	ARG_STRING,
	ARG_PTR,
	ARG_STRUCT,
};

/* Argumet descriptors */
struct arg_desc {
	enum arg_type type;
	const char *name;
	int mandatory;
};

/* Function descriptors */
struct func_desc {
	const char *name;
	int (*fn)(void **args);
	int argc;
	struct arg_desc *argv;
};

int fn_getaddrinfo(void **args);
int fn_getnameinfo(void **args);
