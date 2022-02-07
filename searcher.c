//
// Created by maciej on 2/3/22.
//

#include<unistd.h>
#include<sys/stat.h>
#include <stdio.h>

#include "number_parser.h"
#include "set.h"
#include "number_record.h"


int main(int argc, char *argv[]) {
    if (argc != 2)
        return 11;
    ll input_length = parse_number(argv[1]);
    struct stat pipe_check_result;

    if (fstat(STDIN_FILENO, &pipe_check_result) < 0) {
        perror(NULL);
        return 12;
    }
    if (!S_ISFIFO(pipe_check_result.st_mode))
        return 13;

    set set;
    set_create(&set, 1024);

    number_record record;

    for (ll i = 0; i < input_length; i++) {

        if (read(STDIN_FILENO, &record.number, 2) < 0) {
            perror(NULL);
            return 14;
        }

        if (!set_add(&set, record.number)) {

            record.number = record.number;
            record.pid = getpid();
            write(1, &record, sizeof(record));

        }

    }
    return (int) ((set_count(&set) * 10 - 1) / input_length + 1);
}