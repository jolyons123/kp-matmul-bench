typedef struct mat_arg{
    unsigned int m;
    unsigned int n;
    unsigned int q;
    unsigned int a;
    unsigned int b;
} mat_arg;

int parse_args(int argc, char* argv[], mat_arg* args);
const char* print_usage();