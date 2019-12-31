#include "client/client_cmd.h"

using namespace std;

extern CmdParser* cirMgr;

bool
initCirCmd()
{
    return true;
}

CmdExecStatus
CmdClientRefresh::exec(const string& option)
{
    return CMD_EXEC_DONE;
}

void
CmdClientRefresh::usage(ostream& os) const
{

}

void
CmdClientRefresh::help() const
{
    
}