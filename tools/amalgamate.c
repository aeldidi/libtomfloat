// SPDX-License-Identifier: 0BSD
// Copyright (C) 2022 Ayman El Didi
#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Inlines all #includes which use quotation marks ("") ignoring includes which
// use angle brackets (<>). Given a filename without an extension, outputs an
// amalgamated .c and .h file.
//
// All includes are relative to the file being amalgamated.

#define MAX(x, y) ((x) > (y) ? (x) : (y))

static const char* c_files[] = {
		"mpf_inv.c",
		"mpf_mul_d.c",
		"mpf_const_2rpi.c",
		"mpf_const_pi.c",
		"mpf_const_sqrt_d.c",
		"mpf_neg.c",
		"mpf_const_le2.c",
		"mpf_const_pi4.c",
		"mpf_const_l2e.c",
		"mpf_sub.c",
		"mpf_cos.c",
		"mpf_const_pi2.c",
		"mpf_const_l10e.c",
		"mpf_sub_d.c",
		"mpf_iterations.c",
		"mpf_normalize_to.c",
		"mpf_div.c",
		"mpf_acos.c",
		"mpf_invsqrt.c",
		"mpf_pow.c",
		"mpf_init_multi.c",
		"mpf_const_1pi.c",
		"mpf_ln.c",
		"mpf_add.c",
		"mpf_sqrt.c",
		"mpf_const_r2.c",
		"mpf_tan.c",
		"mpf_abs.c",
		"mpf_const_1r2.c",
		"mpf_sin.c",
		"mpf_clear.c",
		"mpf_const_0.c",
		"mpf_const_d.c",
		"mpf_const_2pi.c",
		"mpf_exch.c",
		"mpf_cmp_d.c",
		"mpf_exp.c",
		"mpf_normalize.c",
		"mpf_div_d.c",
		"mpf_cmp.c",
		"mpf_const_ln_d.c",
		"mpf_add_d.c",
		"mpf_mul_2.c",
		"mpf_asin.c",
		"mpf_mul.c",
		"mpf_clear_multi.c",
		"mpf_div_2.c",
		"mpf_init_copy.c",
		"mpf_init.c",
		"mpf_atan.c",
		"mpf_sqr.c",
		"mpf_const_e.c",
		"mpf_copy.c",
		"libtommath-1.2.0/tommath.c",
};
#define NUM_C_FILES (sizeof(c_files) / sizeof(*c_files))

#define FNV_64_PRIME   UINT64_C(0x100000001b3)
#define FNV_64_INITIAL UINT64_C(0xcbf29ce484222325)

uint64_t
fnv_1a(const size_t buf_len, const uint8_t* buf)
{
	uint64_t hash = FNV_64_INITIAL;

	for (size_t i = 0; i < buf_len; i += 1) {
		hash = (hash ^ buf[i]) * FNV_64_PRIME;
	}

	return hash;
}

static char*
vformat(const char* fmt, va_list args)
{
	// The v*printf functions destroy the va_list after use.
	// So I have to copy the va_list for the second call or we segfault.
	va_list ap;
	va_copy(ap, args);
	int len = vsnprintf(NULL, 0, fmt, args);
	if (len < 0 || len == INT_MAX) {
		return NULL;
	}

	char* result = calloc(len + 1, 1);
	if (result == NULL) {
		return NULL;
	}

	(void)vsnprintf(result, len + 1, fmt, ap);
	va_end(ap);
	return result;
}

static char*
format(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char* result = vformat(fmt, ap);
	va_end(ap);
	return result;
}

static uint8_t*
readfull(char* filename)
{
	FILE* f = fopen(filename, "r");
	if (f == NULL) {
		return NULL;
	}

	uint8_t* result = calloc(2, 1);
	if (result == NULL) {
		fclose(f);
		return result;
	}

	int    c = 0;
	size_t i = 0;
	for (;;) {
		c = fgetc(f);
		if (c == EOF) {
			break;
		}
		result[i]     = c & 0xff;
		result[i + 1] = '\0';
		i += 1;
		void* ptr = realloc(result, i + 2);
		if (ptr == NULL) {
			free(result);
			fclose(f);
			return NULL;
		}
		result = ptr;
	}

	fclose(f);
	return result;
}

#define MAX_INCLUDES 1000

bool
file_has_been_included(const size_t str_len, const char* str,
		const size_t hashes_len, const uint64_t* hashes)
{
	uint64_t hash = fnv_1a(str_len, (uint8_t*)str);
	for (size_t i = 0; i < hashes_len; i += 1) {
		if (hash == hashes[i]) {
			return true;
		}
	}

	return false;
}

// Amalgamates file in dir into f, using pool to check if a file has already
// been included.
void
amalgamate_file(const char* dir, const char* file, FILE* f, size_t* hashes_len,
		uint64_t* hashes)
{
	char* filename = format("%s/%s", dir, file);
	assert(filename != NULL);
	char* content = (char*)readfull(filename);
	assert(content != NULL);
	free(filename);

	char* prev = content;
	char* ptr  = content;
	while ((ptr = strstr(ptr, "#include \"")) != NULL) {
		(void)fprintf(f, "%.*s", (int)(ptr - prev), prev);
		ptr += strlen("#include \"");

		char* closing_quote = strchr(ptr, '"');
		assert(closing_quote != NULL);

		size_t include_len = closing_quote - ptr;
		if (file_has_been_included(
				    include_len, ptr, *hashes_len, hashes)) {
			prev = closing_quote + 2;
			continue;
		}

		assert(*hashes_len + 1 < MAX_INCLUDES);
		hashes[*hashes_len] = fnv_1a(include_len, (uint8_t*)ptr);
		*hashes_len += 1;

		char* fname = format("%.*s", closing_quote - ptr, ptr);
		assert(fname != NULL);
		amalgamate_file(dir, fname, f, hashes_len, hashes);
		free(fname);

		prev = closing_quote + 2;
	}

	(void)fprintf(f, "%s", prev);
	free(content);
}

int
main(int argc, char** argv)
{
	if (argc != 2) {
		(void)fprintf(stderr,
				"usage: %s [src directory]\n"
				"Amalgamates tomfloat.c and tomfloat.h given "
				"the source directory.\n"
				"All includes are resolved relative to the "
				"directory given.\n",
				argv[0]);
		return EXIT_FAILURE;
	}

	FILE* f = fopen("tomfloat.c", "w");
	assert(f != NULL);

	static uint64_t include_hashes[MAX_INCLUDES];
	size_t          include_hashes_len = 0;

	for (size_t i = 0; i < NUM_C_FILES; i += 1) {
		amalgamate_file(argv[1], c_files[i], f, &include_hashes_len,
				include_hashes);
	}

	fclose(f);

	f = fopen("tomfloat.h", "w");
	assert(f != NULL);

	include_hashes_len = 0;
	amalgamate_file(argv[1], "tomfloat.h", f, &include_hashes_len,
			include_hashes);

	fclose(f);
}
