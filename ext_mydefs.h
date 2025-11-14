#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define H 4
#define W 5

typedef struct {
    unsigned char a[H][W];
} board;

#ifndef MAXLIST
#define MAXLIST 200000
#endif
