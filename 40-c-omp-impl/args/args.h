typedef struct mat_arg{
    unsigned int m;
    unsigned int n;
    unsigned int q;
    unsigned int row_split;
    unsigned int col_split;
    unsigned int max_float;
} mat_arg;

int parse_args(int argc, char* argv[], mat_arg* args);
void print_usage();