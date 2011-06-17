
#ifndef __REQUEST_COMMAND_H__
#define __REQUEST_COMMAND_H__

#include "TCommand.h"

class TRequestCommand
{
public:
    TRequestCommand(const TCommand &aCmd)
        : iCmd(aCmd)
    {
        iRet = KErrNone;
    }
    TRequestCommand()
    {
        iRet = KErrNone;
    }

public:
    TInt iRet;
    TCommand iCmd;
};

#endif  //__REQUEST_COMMAND_H__
