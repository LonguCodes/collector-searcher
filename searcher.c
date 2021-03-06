//
// Created by maciej on 2/3/22.
//

#include<unistd.h>
#include<sys/stat.h>
#include <signal.h>
#include <stdio.h>


#include "number_parser.h"
#include "set.h"
#include "number_record.h"
#include "throw_exception.h"

#define E_INVALID_ARG 11
#define E_FSTAT 12
#define E_STDIN_PIPE 13
#define E_PIPE_READ 14
#define E_PIPE_WRITE 15

void die_on_signal(int i) {
    _exit(0);
}

int ceil(double d) {
    int whole = (int) d;
    if (d - whole > 0)
        return whole + 1;
    return whole;
}

int main(int argc, char *argv[]) {

    if (argc != 2)
        return E_INVALID_ARG;
    ll input_length = parse_number(argv[1]);
    struct stat pipe_check_result;

    throw_exception(fstat(STDIN_FILENO, &pipe_check_result), E_FSTAT);
    if (!S_ISFIFO(pipe_check_result.st_mode))
        return E_STDIN_PIPE;

    signal(SIGUSR1, die_on_signal);

    set set;
    set_create(&set, 1024);

    number_record record;

    int repeats = 0;
    for (ll i = 0; i < input_length; i++) {
        long long rResult = read(STDIN_FILENO, &record.number, 2);
        throw_exception(rResult, E_PIPE_READ);


        if (!set_add(&set, record.number)) {

            record.number = record.number;
            record.pid = getpid();
            throw_exception(write(1, &record, sizeof(record)), E_PIPE_WRITE);
        } else
            repeats++;
    }

    return ceil(10.0 * repeats / input_length);
}