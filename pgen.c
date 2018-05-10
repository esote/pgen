/* pgen -- generate cryptographically secure random strings */

#include <error.h>
#include <getopt.h>
#include <inttypes.h>
#include <sodium.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROGRAM_NAME "pgen"

#define _warn(...) do {					\
		if(!quiet)						\
			error(0, 0, __VA_ARGS__);	\
	} while(0)

#define STRTOUMAX_BASE 10

#define LOWER "abcdefghijklmnopqrstuvwxyz"
#define LOWER_L 26

#define NUMERIC "0123456789"
#define NUMERIC_L 10

#define SPECIAL "!@#$%^&*()-_=+`~[]{}\\|;:'\",.<>/?"
#define SPECIAL_L 32

#define UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define UPPER_L 26

/* Disable warnings */
static bool quiet;

static struct option const long_opts[] = {
	{"help", no_argument, NULL, 'h'},
	{"lower", no_argument, NULL, 'L'},
	{"numeric", no_argument, NULL, 'N'},
	{"quiet", no_argument, NULL, 'q'},
	{"special", no_argument, NULL, 'S'},
	{"upper", no_argument, NULL, 'U'},
	{NULL, 0, NULL, 0}
};

_Noreturn void usage(int const status)
{
	if(status != EXIT_SUCCESS) {
		fprintf(stderr, "Usage: %s [OPTION]...\n", PROGRAM_NAME);
		fprintf(stderr, "Try '%s --help' for more information.\n",
				PROGRAM_NAME);
	} else {
		printf("Usage: %s [LEN] [OPTION]...\n", PROGRAM_NAME);
		puts("Generate cryptographically secure strings.");
		printf("Example: %s 16 -LNSU\n", PROGRAM_NAME);
		puts("\nOptions:\n\
  -h, --help       display this help text and exit\n\
  -L, --lower      generate lowercase letters\n\
  -N, --numeric    generate numeric digits\n\
  -q, --quiet      disable warnings\n\
  -S, --special    generate special characters\n\
  -U, --upper      generate uppercase letters");
	}

	exit(status);
}

int main(int const argc, char *const *const argv)
{
	char const *const key = (argc > 1) ? argv[1] : NULL;

	if(key != NULL
       && (strcmp(key, "-h") == 0 || strcmp(key, "--help") == 0)) {
		usage(EXIT_SUCCESS);
	}

	if(key == NULL)
		error(EXIT_FAILURE, 0, "first argument must be length");

	/* First argument is positional */
	optarg++;

	quiet = false;

	bool lower_b = false,
		 upper_b = false,
		 numeric_b = false,
		 special_b = false;

	uintmax_t const length = strtoumax(key, NULL, STRTOUMAX_BASE);

	uint32_t chars_l = 0;

	int c;

	while((c = getopt_long(argc, argv, "hLNqSU", long_opts, NULL)) != -1) {
		switch(c) {
			case 'h':
				usage(EXIT_SUCCESS);
				break;
			case 'L':
				if(!lower_b) {
					chars_l += LOWER_L;
					lower_b = true;
				}
				break;
			case 'N':
				if(!numeric_b) {
					chars_l += NUMERIC_L;
					numeric_b = true;
				}
				break;
			case 'q':
				quiet = true;
				break;
			case 'S':
				if(!special_b) {
					chars_l += SPECIAL_L;
					special_b = true;
				}
				break;
			case 'U':
				if(!upper_b) {
					chars_l += UPPER_L;
					upper_b = true;
				}
				break;
			default:
				usage(EXIT_FAILURE);
		}
	}

	if(length == 0) {
		_warn("length is zero");
		exit(EXIT_SUCCESS);
	}

	if(lower_b + numeric_b + special_b + upper_b == 0) {
		_warn("no character sets selected");
		exit(EXIT_SUCCESS);
	}

	if(key[0] == '-' && !quiet) {
		_warn("it appears you entered a negative value, "
			  "this value will be evaluated in an unsigned context");
		sleep(2);
	}

	char *const chars = malloc(chars_l + 1);

	if(chars == NULL)
		error(EXIT_FAILURE, 0, "failed to allocate memory for string");

	chars[0] = '\0';

	if(lower_b)
		strncat(chars, LOWER, LOWER_L);

	if(numeric_b)
		strncat(chars, NUMERIC, NUMERIC_L);

	if(special_b)
		strncat(chars, SPECIAL, SPECIAL_L);

	if(upper_b)
		strncat(chars, UPPER, UPPER_L);

	for(size_t i = 0; i < length; ++i)
		putchar(chars[randombytes_uniform(chars_l)]);

	free(chars);

	putchar('\n');

	return EXIT_SUCCESS;
}

