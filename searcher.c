//
// Created by maciej on 2/3/22.
//

#include<unistd.h>
#include<sys/stat.h>

#include "number_parser.h"
#include "set.h"

struct number_record {
    short number;
    pid_t pid;
};

int main(int argc, char *argv[]) {
    if (argc != 2)
        return 11;
    ll input_length = parse_number(argv[1]);
    struct stat pipe_check_result;

    fstat(0, &pipe_check_result);

    if (!S_ISFIFO(pipe_check_result.st_mode))
        return 12;

    set set;
    set_create(&set, 1024);

    for (ll i = 0; i < input_length; i++) {
        short v;
        read(0, &v, 2);
        if (!set_add(&set, v)) {

            struct number_record record;
            record.number = v;
            record.pid = getpid();
            write(1, &record, sizeof(record));
        }
    }
    return (int) (set_count(&set) * 10 / input_length);
}