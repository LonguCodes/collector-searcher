//
// Created by Maciej Łyskawiński Private on 04/02/2022.
//
#include <printf.h>
#include "unistd.h"
#include "number_parser.h"


int main(int argc, char* argv[]) {


    char * input_file_path,* result_file_path, *to_process_per_child;
    ll to_load;
    int max_number_of_children;
    int ch;
    while ((ch = getopt(argc, argv, "d:s:w:f:l:p:")) != -1) {
        switch (ch) {

            case 'd':
                input_file_path = optarg;
                break;
            case 's':
                to_load = parse_number(optarg);
                break;
            case 'w':
                to_process_per_child = optarg;
                break;
            case 'f':
                result_file_path = optarg;
                break;
            case 'p':
                max_number_of_children = (int)parse_number(optarg);
            case '?':
            default:
                printf("Wrong argument");
                break;
        }
    }


    char* args[3];
    args[0] = "searcher.c";
    args[1] = to_process_per_child;
    args[2] = NULL;
    int pipeIn[2];
    int pipeOut[2];
    pipe(pipeIn);
    pipe(pipeOut);

    int fork_result = 0;


    for (int i = 0; i < max_number_of_children && !fork_result; ++i) {
        fork_result = fork();
        if(fork_result)
            continue;

        dup2(0, pipeIn[1]);
        dup2(1, pipeOut[0]);

        close(pipeIn[0]);
        close(pipeOut[1]);

        execv("searcher.c", args);
    }






}