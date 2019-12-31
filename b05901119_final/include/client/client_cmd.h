#ifndef CLIENT_CMD_H
#define CLIENT_CMD_H

#include "cmd/cmdParser.h"

CmdClass(CmdClientLogin);
CmdClass(CmdClientRegister);
CmdClass(CmdClientSend);
CmdClass(CmdClientView);
CmdClass(CmdClientRefresh);

#endif