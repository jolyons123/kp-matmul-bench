typedef struct mat_arg{
    unsigned int m;
    unsigned int n;
    unsigned int q;
    unsigned int row_split;
    unsigned int col_split;
} mat_arg;

int parse_args(int argc, char* argv[], mat_arg* args);
const char* print_usage();