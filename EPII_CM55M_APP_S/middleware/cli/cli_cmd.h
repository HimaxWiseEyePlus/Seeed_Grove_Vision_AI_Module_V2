#ifndef CLI_CMD_H_
#define CLI_CMD_H_

void CLI_AddResultData(const char *fmt, ...);
void CLI_SetResultData(const char *fmt, ...);
void CLI_SetStatusCode(int status);
#endif /*CLI_CMD_H_*/
