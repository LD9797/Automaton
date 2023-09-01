int DFA_driver (
    int **Table,
    int *Accept,
    int(*code)(char c),
    char *string,
    int state)
    {
        char * s;
        int k;

        s = string;
        k = state;

        while (*s)
        k = Table [k][code(*ss)];
        
    return (Accept[k]);
    }