#include <stdio.h>
#include <stdlib.h>

#include "args.h"

void print_usage();

int parse_args(int argc, char* argv[], mat_arg* args){
    unsigned short i;
    for (i = 1; i < argc; i++) {
        if(argv[i][0] != '-') continue;
        
        // check if value comes after parameter name
        if (argc < i + 2) {
            fprintf(stderr, "Could not find value for argument with name: %s\n", argv[i]);
            print_usage();
            return EXIT_FAILURE;
        }
        unsigned short value = atoi(argv[i+1]);
        // fill the args struct
        switch (argv[i][1]) {
            case 'm': args->m = value; break;
            case 'n': args->n = value; break;
            case 'q': args->q = value; break;
            case 'a': args->row_split = value; break;
            case 'b': args->col_split = value; break;
            case 'v': args->max_float = value; break;
            default:
                print_usage();
                return EXIT_FAILURE;
        }   
    }

    return EXIT_SUCCESS;
}

void print_usage(){
    fprintf(stderr, "Usage: {executable} [[-mnqabv] <value>, ..]]\n\tMultiply matrix A (<m> rows and <n> columns) with matrix B (<n> rows and <q> columns)\n\tsplitting matrix A alongside its rows by <a> and alongside its columns by <b>.\n\tInitialize matrices A and B with random float32 not exceeding <v>");
}