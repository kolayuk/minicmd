
#ifndef __MINI_CMD_SESSION_H__
#define __MINI_CMD_SESSION_H__

#include <e32base.h>

#include "MiniCmdServer.h"
#include "RequestCmd.h"

class CMCServerSession : public CSession2
{
public:
    static CMCServerSession* NewL(CMiniCmdServer& aServer);
    static CMCServerSession* NewLC(CMiniCmdServer& aServer);
    virtual ~CMCServerSession();

public:
    void ServiceL(const RMessage2& aMessage);

private:

    CMCServerSession(CMiniCmdServer& aServer);
    void ConstructL();

private:

    void PanicClient(const RMessagePtr2& aMessage, TInt aPanic) const;
    void RequestCommandL(const RMessage2& aMessage);
    
    TInt DoCommandL(const TCommand &aCmd);
    
private:

    CMiniCmdServer& iServer;
};

#endif // __MINI_CMD_SESSION_H__
///////////////////////////////////////////////////////////////////////////////////
//END OF FILE
///////////////////////////////////////////////////////////////////////////////////
