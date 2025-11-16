#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>

#define ERROR_EXIT(_e, ...) if (!_e) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }
#define ERROR_RESULT(_e, _r, ...) if (_e != _r) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }

#define MAXLINE 4096
#define SERVER_PORT 18000

#endif // __UTILS_H__
