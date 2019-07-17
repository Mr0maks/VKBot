#include "cmds.h"

#include <strings.h>

typedef struct
{
  char	*fmt_string;
  void	(*function)(int argc, char **argv);
} cmds_t;

typedef struct
{
  unsigned int hash;
  void	(*function)(int argc, char **argv);
} cmds_hashs_t;

void cmd_help()
{
  
}

cmds_t commands[] = {
  {"%s помощь", cmd_help}
};

void cmd_handler_init(const char *name)
{
  
}
