//
// Created by maciej on 2/3/22.
//

#pragma once

#include <stdlib.h>
#include <string.h>

typedef struct {
    short **items;
    int *counts;
    int *sizes;
    int size;
} set;

void set_create(set *stk, int size);

int set_has(set *stk, short item);

int set_add(set *stk, short item);

long long set_count(set *set);
