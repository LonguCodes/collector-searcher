//
// Created by maciej on 2/3/22.
//

#include "set.h"

void set_create(set *stk, int size) {
    stk->items = calloc(size, sizeof(short *));
    stk->counts = calloc(size, sizeof(int));
    stk->sizes = calloc(size, sizeof(int));
    stk->size = size;
    for (int i = 0; i < size; i++) {
        stk->items[i] = calloc(3, sizeof(short));
        stk->counts[i] = 0;
        stk->sizes[i] = 3;
    }
}

int set_has(set *stk, short item) {
    int index = item % stk->size;
    for (int i = 0; i < stk->counts[index]; ++i) {
        if (stk->items[index][i] == item)
            return 1;
    }
    return 0;
}

int set_add(set *stk, short item) {
    if (set_has(stk, item))
        return 1;
    int index = item % stk->size;
    if (stk->counts[index] == stk->sizes[index]) {
        short *newArr = calloc(stk->sizes[index] * 2 + 1, sizeof(short));
        stk->sizes[index] = stk->sizes[index] * 2 + 1;
        memcpy(newArr, stk->items[index], stk->counts[index] * sizeof(short));
    }
    stk->items[index][stk->counts[index]] = item;
    stk->counts[index]++;
    return 0;
}

long long set_count(set *set) {
    long long count = 0;
    for (int i = 0; i < set->size; ++i) {
        count += set->counts[i];
    }
    return count;
}