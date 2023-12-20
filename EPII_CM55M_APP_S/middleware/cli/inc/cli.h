#ifndef __CLI_H__
#define __CLI_H__

#include "cli_type.h"

typedef BOOL_T (*CLI_CMD_HANDLER_T)(int argc, char *args[]);

void CLI_InitMenu(void);
void CLI_EnablePassword(BOOL_T yes);
void CLI_Execute(const char *cmd);
void CLI_GetCurrentPath(char *pwd, int pwd_size);
void CLI_CommandLoop(void);

#endif /*__CLI_H__*/
