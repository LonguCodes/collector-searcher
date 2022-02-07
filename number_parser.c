//
// Created by maciej on 2/3/22.
//

#include <string.h>
#include <stdlib.h>
#include "number_parser.h"

ll parse_number(const char *s) {
    char *rest;
    ll number_part = strtol(s, &rest, 10);
    if (!strcmp(rest, "Ki"))
        number_part <<= 10;
    else if (!strcmp(rest, "Mi"))
        number_part <<= 20;
    else if (!strcmp(rest, "Gi"))
        number_part <<= 30;
    else if (!strcmp(rest, "Ti"))
        number_part <<= 40;
    return number_part;
}

