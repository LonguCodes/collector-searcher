//
// Created by Maciej Łyskawiński Private on 04/02/2022.
//
#include <printf.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <poll.h>

#include "number_parser.h"
#include "number_record.h"
#include "set.h"

#define BUF_SIZE 16384
#define SHORT_MAX 65536


#define E_INPUT_FILE 1
#define E_FCNTL 2
#define E_READ_FILE 3
#define E_WRITE_PIPE 4
#define E_LOG_FILE 5
#define E_RESULT_FILE 6


int current_children_count = 0;
int max_number_of_children;
ll yet_to_load;
ll to_load;

set found_numbers;

int log_file = -1;
int result_file = -1;

ll min(ll a, ll b) {
    return a < b ? a : b;
}

void write_result_file(int number, pid_t pid) {
    lseek(result_file, number * sizeof(pid_t), SEEK_SET);
    pid_t current;
    if (read(result_file, &current, sizeof(pid_t)) < 0) {
        perror(NULL);
        exit(E_LOG_FILE);
    }
    if (current != 0)
        return;

    lseek(result_file, number * sizeof(pid_t), SEEK_SET);

    if (write(result_file, &pid, sizeof(pid_t)) < 0) {
        perror(NULL);
        exit(E_LOG_FILE);
    }
}

void log_child_update(char *status, int pid) {
    char log_buff[2048];
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    sprintf(log_buff, "%lld.%.9ld, %s, %d\n", (ll) time.tv_sec, time.tv_nsec, status, pid);

    if (write(log_file, log_buff, strlen(log_buff)) < 0) {
        perror(NULL);
        exit(E_LOG_FILE);
    }
}

int check_dead_children() {
    int any_died = 0;
    int pid, status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {

        if (!WIFEXITED(status))
            continue;

        log_child_update("DIE", pid);

        any_died = 1;
        current_children_count--;
        if (WEXITSTATUS(status) > 10 || set_count(&found_numbers)  > SHORT_MAX * 0.75)
            max_number_of_children--;
    }
    return any_died;
}

void create_child(int pipeIn[2], int pipeOut[2], char *to_process_per_child) {
    int fork_result = fork();
    if (fork_result < 0) {
        perror(NULL);
        exit(9);
    }
    if (fork_result) {
        current_children_count++;
        log_child_update("CREATE", fork_result);
        return;
    }

    char *args[3];
    args[0] = "searcher";
    args[1] = to_process_per_child;
    args[2] = NULL;


    dup2(pipeIn[0], 0);
    dup2(pipeOut[1], 1);

    int exec_result = execv("searcher", args);
    if (exec_result != 0) {
        perror(NULL);
        exit(10);
    }

}

int main(int argc, char *argv[]) {


    char *input_file_path, *result_file_path, *log_file_path, *to_process_per_child;
    int ch;
    while ((ch = getopt(argc, argv, "d:s:w:f:l:p:")) != -1) {
        switch (ch) {
            case 'd':
                input_file_path = optarg;
                break;
            case 's':
                to_load = parse_number(optarg);
                yet_to_load = to_load;
                break;
            case 'w':
                to_process_per_child = optarg;
                break;
            case 'f':
                result_file_path = optarg;
                break;
            case 'l':
                log_file_path = optarg;
                break;
            case 'p':
                max_number_of_children = (int) parse_number(optarg);
                break;
            case '?':
            default:
                fprintf(stderr, "Wrong argument %s\n", optarg);
                break;
        }
    }

    int input_file = open(input_file_path, O_RDONLY);
    if (input_file == -1) {
        perror(NULL);
        return E_INPUT_FILE;
    }
    log_file = open(log_file_path, O_WRONLY | O_CREAT | O_APPEND);
    if (log_file == -1) {
        perror(NULL);
        return E_LOG_FILE;
    }

    result_file = open(result_file_path, O_RDWR | O_CREAT | O_TRUNC);
    if (result_file == -1) {
        perror(NULL);
        return E_RESULT_FILE;
    }

    pid_t empty[65536] = {0};
    write(result_file, empty, sizeof(empty));

    set_create(&found_numbers, 1024);

    int pipeIn[2];
    int pipeOut[2];

    pipe(pipeIn);
    pipe(pipeOut);

    int result = fcntl(pipeOut[0], F_SETFL, O_NONBLOCK);
    if (result < 0) {
        perror(NULL);
        return E_FCNTL;
    }

    result = fcntl(pipeIn[1], F_SETFL, O_NONBLOCK);
    if (result < 0) {
        perror(NULL);
        return E_FCNTL;
    }

    char buffer[BUF_SIZE];
    int write_result = BUF_SIZE;

    number_record record;

    struct timespec to_wait;
    to_wait.tv_sec = 0;
    to_wait.tv_nsec = 48e+7l;

    signal(SIGCHLD, NULL);

    while (max_number_of_children > 0) {

        int any_read = 0;
        while (current_children_count < max_number_of_children) {
            create_child(pipeIn, pipeOut, to_process_per_child);
        }

        if (write_result == BUF_SIZE && yet_to_load > 0) {

            size_t file_read_result = read(input_file, buffer, min(BUF_SIZE, yet_to_load));
            if (file_read_result < 0) {
                perror(NULL);
                return E_READ_FILE;
            }
            yet_to_load -= (int) file_read_result;
            yet_to_load = yet_to_load > 0 ? yet_to_load : 0;


            write_result = 0;
        }

        int res = (int) write(pipeIn[1], buffer + write_result, BUF_SIZE - write_result);
        if (res < 0 && errno != EAGAIN) {
            perror(NULL);
            return E_WRITE_PIPE;
        }
        if (res > 0)
            write_result += res;


        while (read(pipeOut[0], &record, sizeof(record)) > 0) {
            any_read = 1;
            write_result_file(record.number, record.pid);
            set_add(&found_numbers, record.number);
        }

        int any_died = check_dead_children();
        if (!any_read && !any_died) {
            printf("Nothing happened, sleeping...\n");
            nanosleep(&to_wait, NULL);
        }

        if (yet_to_load == 0 && !poll(&(struct pollfd) {.fd = pipeIn[0], .events = POLLIN}, 1, 0))
            break;
    }
}