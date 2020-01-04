#include "common.h"

typedef void (*console_handle_t)(int argc, char **argv);

typedef struct
{
    const char *string;
    const char *desc;
    console_handle_t handle;
} console_cmds_t;

void cmd_hello(int argc, char **argv)
{
    Con_Printf("Hello World from bot console!\n");
}

void Host_Exit(void);

void cmd_exit(int argc, char **argv)
{
    Host_Exit();
}

static const console_cmds_t cmds[] = {
    {"hello", "hello world", cmd_hello},
    {"exit", "exit from bot", cmd_exit},
    {NULL, NULL, NULL}
};

static console_handle_t console_find_cmd(const char *cmd)
{
for(int i = 0; cmds[i].string != NULL; i++)
{
    if(!strcasecmp(cmd, cmds[i].string))
        return cmds[i].handle;
}

    return NULL;
}

int console_string_tokeinize(char *str, char *tokens[], int *tokens_len );

void console_handler(const char *string)
{
    if(string[0] == '\n')
        return;

    char **tokens = (char**)calloc(256, sizeof(*tokens));

    char *string_copy = strdup(string);

    int tokens_count = 0;

    console_string_tokeinize( string_copy, tokens, &tokens_count );

    console_handle_t handle = console_find_cmd(tokens[0]);

    if(!handle)
        return;

    handle(tokens_count - 1, tokens);
}
