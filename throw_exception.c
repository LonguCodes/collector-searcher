//
// Created by maciej on 2/7/22.
//


#include <stdio.h>
#include <stdlib.h>
#include "throw_exception.h"


ll throw_exception(ll result, int error) {
    if (result == -1) {
        perror(NULL);
        exit(error);
    }
    return result;
}