#include "main.h"

int main() {
    char prompt[25] = "minishell$";
    char input_string[1024];

    scan_input(prompt, input_string);
    return 0;
}