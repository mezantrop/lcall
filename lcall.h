/*
 * SPDX-License-Identifier: BSD-2-Clause
 * Copyright (c) 2026 Mikhail Zakharov
 */

#define NARGS(ARR) (sizeof(ARR)/sizeof(ARR[0]))

/* Argumet descriptors */
struct arg_desc {
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

#define LCALL_VERSION "1.1"
