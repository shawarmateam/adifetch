#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include "config.def.h"

#define RESET "\033[0m"

char* readFile(const char* filename) {
    FILE *file;
    char line[256];
    char *content = NULL;
    size_t total_length = 0;

    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Can't open the theme file: \"%s\".\n", filename);
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), file)) {
        size_t line_length = strlen(line);

        char *new_content = realloc(content, total_length + line_length + 1);
        content = new_content;

        strcpy(content + total_length, line);
        total_length += line_length;
    }

    fclose(file);
    return content;
}

char** splitString(const char *input, int *count) {
    char *input_copy = strdup(input);
    if (!input_copy) {
        return NULL;
    }

    char **args = NULL;
    *count = 0;

    char *token = strtok(input_copy, " ");
    while (token != NULL) {
        args = (char **)realloc(args, (*count + 1) * sizeof(char *));
        if (!args) {
            free(input_copy);
            return NULL;
        }

        args[*count] = strdup(token);
        if (!args[*count]) {
            free(input_copy);
            for (int i = 0; i < *count; i++) {
                free(args[i]);
            }
            free(args);
            return NULL;
        }

        (*count)++;
        token = strtok(NULL, " ");
    }

    free(input_copy);
    return args;
}


void get_os_info(char *os_name, size_t size) {
    struct utsname buffer;
    uname(&buffer);
    snprintf(os_name, size, "%s %s", buffer.sysname, buffer.release);
}

void get_shell_info(char *shell_name, size_t size) {
    char *shell = getenv("SHELL");
    if (shell) {
        snprintf(shell_name, size, "%s", strrchr(shell, '/') + 1);
    } else {
        snprintf(shell_name, size, "unknown");
    }
}

void get_kernel_info(char *kernel_version, size_t size) {
    struct utsname buffer;
    uname(&buffer);
    snprintf(kernel_version, size, "%s", buffer.release);
}

void get_ram_usage(char *ram_usage, size_t size) {
    FILE *fp = popen("free -h | awk '/^Mem/ { print $3\"/\"$2 }' | sed s/i//g", "r");
    if (fp) {
        fgets(ram_usage, size, fp);
        pclose(fp);
    } else {
        snprintf(ram_usage, size, "???");
    }
}

void get_host(char *host, size_t size) {
    FILE *fp = popen("cat /etc/hostname", "r");
    if (fp) {
        fgets(host, size, fp);
        pclose(fp);
    } else {
        snprintf(host, size, "unknown");
    }
}

void get_wm(char *wm, size_t size) {
    FILE *fp = popen("sh ~/apps/c/adifetch/GET_WM_CMD", "r");

    if (fp) {
        fgets(wm, size, fp);
        pclose(fp);
    } else {
        snprintf(wm, size, "???");
    }
}

int main() {
    char os_name[256];
    char shell_name[256];
    char wm[256];
    char kernel_version[256];
    char ram_usage[256];
    char *username = getenv("USER");
    char host[256];

    get_os_info(        os_name,        sizeof(os_name));
    get_shell_info(     shell_name,     sizeof(shell_name));
    get_wm(             wm,             sizeof(wm));
    get_kernel_info(    kernel_version, sizeof(kernel_version));
    get_ram_usage(      ram_usage,      sizeof(ram_usage));
    get_host(           host,           sizeof(host));

    printf(ICON0 "           ██           " NAME "%s" AT "@" HOST "%s", username, host);
    printf(ICON1 "          ████          " RESET "──────────────────\n");
    printf(ICON2 "          ▀████         \n" RESET);
    printf(ICON3 "        ██▄ ████        " EMOJI " " RESET " cores " ARROW "" VAL " %d\n", sysconf(_SC_NPROCESSORS_ONLN));
    printf(ICON4 "       ██████████       " EMOJI " " RESET " sh    " ARROW "" VAL " %s\n" RESET, shell_name);
    printf(ICON5 "      ████▀  ▀████      " EMOJI " " RESET " wm    " ARROW "" VAL " %s", wm);
    printf(ICON6 "     ████▀    ▀████     " EMOJI " " RESET " kr&os " ARROW "" VAL " %s\n", os_name);
    printf(ICON7 "    ▀▀▀          ▀▀▀    " EMOJI " " RESET " ram   " ARROW "" VAL " %s\n", ram_usage);

    return 0;
}

