#pragma once

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define EXIT_WITH_ERR(err, ...)          \
    fprintf(stderr, err, ##__VA_ARGS__); \
    exit(EXIT_FAILURE);

#define TARGET_REACHED 1
#define TARGET_NOT_REACHED 2
#define EXCEEDED_ANSWER 3
#define NO_ANSWER 4
