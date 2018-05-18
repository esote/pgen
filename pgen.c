/* pgen -- generate cryptographically secure random strings */

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <inttypes.h>
#include <sodium.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct option const long_opts[] = {
	{"help", no_argument, NULL, 'h'},
	{"print-sets", no_argument, NULL, 'p'},
	{"allow-zero", no_argument, NULL, 'z'},

	{"all", no_argument, NULL, 'A'},
	{"lower", no_argument, NULL, 'L'},
	{"numeric", no_argument, NULL, 'N'},
	{"special", no_argument, NULL, 'S'},
	{"upper", no_argument, NULL, 'U'},

	{NULL, 0, NULL, 0}
};

static _Noreturn void usage(int const status, char const *const name)
{
	printf("Usage: %s [OPTION]... LEN\n\n", name);
	if(status != EXIT_SUCCESS) {
		fprintf(stderr, "Try '%s --help' for more information.\n", name);
	} else {
		puts("Generate cryptographically secure strings.\n");
		printf("Example: %s -LN 64\n", name);
		puts("\nOptions:\n\
  -h, --help          display this help text and exit\n\
  -p, --print-sets    print character sets and exit\n\
  -z, --allow-zero    allow zero-length passwords (exiting with success)\n\
  -A, --all           use all character sets (lower, numeric, special, upper)\n\
  -L, --lower         generate lowercase letters\n\
  -N, --numeric       generate numeric digits\n\
  -S, --special       generate special characters\n\
  -U, --upper         generate uppercase letters\n\n\
Default length is 16, default character set is '--all'");
	}

	exit(status);
}

static _Noreturn void print_sets(char const *const lower,
								 char const *const numeric,
								 char const *const special,
								 char const *const upper)
{
	puts("Lower:");
	puts(lower);

	puts("\nNumeric:");
	puts(numeric);

	puts("\nSpecial:");
	puts(special);

	puts("\nUpper:");
	puts(upper);

	exit(EXIT_SUCCESS);
}

struct charset {
	char const *const c;
	uint32_t const l;
	bool b;
};

int main(int const argc, char *const *const argv)
{
	if(sodium_init() < 0)
		error(EXIT_FAILURE, 0, "libsodium could not be initialized");

	bool allow_zero = false;

	struct charset lower = {
		"abcdefghijklmnopqrstuvwxyz",
		(uint32_t)strlen(lower.c), false
	};

	struct charset numeric = {
		"0123456789",
		(uint32_t)strlen(numeric.c), false
	};

	struct charset special = {
		"!@#$%^&*()-_=+`~[]{}\\|;:'\",.<>/?",
		(uint32_t)strlen(special.c), false
	};

	struct charset upper = {
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
		(uint32_t)strlen(upper.c), false
	};

	int c;

	while((c = getopt_long(argc, argv, "hpzALNSU", long_opts, NULL)) != -1) {
		switch(c) {
			case 'h':
				usage(EXIT_SUCCESS, argv[0]);
				break;
			case 'p':
				print_sets(lower.c, numeric.c, special.c, upper.c);
				break;
			case 'z':
				allow_zero = true;
				break;
			case 'A':
				lower.b = numeric.b = special.b = upper.b = true;
				break;
			case 'L':
				lower.b = true;
				break;
			case 'N':
				numeric.b = true;
				break;
			case 'S':
				special.b = true;
				break;
			case 'U':
				upper.b = true;
				break;
			default:
				usage(EXIT_FAILURE, argv[0]);
		}
	}

	char const *const length_tmp = argv[optind++];

	uintmax_t const length = (length_tmp == NULL
							  ? 16
							  : strtoumax(length_tmp, NULL, 10));

	if(length == UINTMAX_MAX && errno == ERANGE)
		error(EXIT_FAILURE, 0, "cannot convert string to integer (overflow)");

	if(length == 0) {
		if(allow_zero) {
			exit(EXIT_SUCCESS);
		} else {
			error(EXIT_FAILURE, 0, "cannot generate zero-length string,"
								   " try '--help'");
		}
	}

	if(!(lower.b || numeric.b || special.b || upper.b))
		lower.b = numeric.b = special.b = upper.b = true;

	uint32_t const chars_l = (lower.b * lower.l)
							 + (numeric.b * numeric.l)
							 + (special.b * special.l)
							 + (upper.b * upper.l);

	char *const chars = malloc(chars_l + 1);

	if(chars == NULL)
		error(EXIT_FAILURE, 0, "failed to allocate memory for string");

	chars[0] = '\0';

	char *endptr = chars;

	if(lower.b) {
		memcpy(endptr, lower.c, lower.l);
		endptr += lower.l;
	}

	if(numeric.b) {
		memcpy(endptr, numeric.c, numeric.l);
		endptr += numeric.l;
	}

	if(special.b) {
		memcpy(endptr, special.c, special.l);
		endptr += special.l;
	}

	if(upper.b)
		memcpy(endptr, upper.c, upper.l);

	for(uintmax_t i = 0; i < length; ++i)
		putchar(chars[randombytes_uniform(chars_l)]);

	free(chars);

	putchar('\n');

	return EXIT_SUCCESS;
}

