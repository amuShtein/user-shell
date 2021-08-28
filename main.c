#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define TRUE 1
#define BUF 32
#define ARG_BUF 8;
#define MAX_ARGS 5

char* join_with_sep(const char* str1, const char* str2, const char* sep) {
    char* res = (char*) malloc(strlen(str1) + strlen(sep) + strlen(str2) + 1);

    strcpy(res, str1);
    strcat(res, sep);
    strcat(res, str2);

    return res;
}

char** split(const char* s, int* n, char separate) {
    int len_buf = 10;
    int buf = ARG_BUF;
    char** out = (char**)malloc(len_buf * sizeof (char*));
    out[0] = (char*)malloc(buf);

    char c;
    int j = 0;
    int k = 0;

    int len = strlen(s);


    for(int i = 0; i < len && (c = s[i]) != '\n'; i++) {
        c = s[i];
        if(c == separate) {
            out[j][k] = 0;

            out[j] = (char *)realloc(out[j], k+1);
            if(out[j] == NULL) {
                printf("ERROR");
            }
//            printf("out[%d] = '%s'\n", j, out[j]);

            j++;
            if (j >= len_buf) {
                len_buf *= 2;
                out = (char**)realloc(out, len_buf * sizeof (char*));
            }

            k = 0;
            buf = ARG_BUF;
            out[j] = (char*)malloc(buf+1);
            if(out[j] == NULL) {
                printf("ERROR");
            }
            continue;
        }

        if (k >= buf-1) {
           buf *= 2;
           out[j] = (char *)realloc(out[j], buf+1);
            if(out[j] == NULL) {
                printf("ERROR");
            }
        }

        out[j][k] = c;
        k++;
    }

    out[j][k] = 0;
    out[j] = (char *)realloc(out[j], k+1);
//    printf("out[%d] = '%s'\n", j, out[j]);

    *n = j+1;
    out = (char**)realloc(out, (*n + 1)*sizeof (char*));
    out[*n] = NULL;

    return out;
}


int main() {

    while (TRUE) {
        printf("enter your command:\n");
        char *command = NULL;
        char *s;

        size_t buf = BUF;
        s = (char *) malloc(buf + 1);

        size_t code = getline(&s, &buf, stdin);

        int argc;
        char **args = split(s, &argc, ' ');

        char *command_name = args[0];
        if(strcmp(command_name, "exit") == 0) {
            break;
        }

        const char *path_name = "PATH";
        const char *path = getenv("PATH");
        char *env[2];

        env[0] = join_with_sep(path_name, path, "=");
        env[1] = 0;

        int envs_len;
        char **envs = split(path, &envs_len, ':');

        char *current_patch;
        for (int i = 0; i < envs_len; i++) {

            current_patch = join_with_sep(envs[i], command_name, "/");
            if (access(current_patch, X_OK) != -1) {
                command = current_patch;
            }
        }

        if (command != NULL) {
            printf("command %s founded!\n", command);
            if(fork() != 0) {
                int status;
                waitpid(-1, &status, 0);
            } else {
                execve(command, args, env);
            }
        } else {
            printf("command not founded!\n");
        }
    }
}
