

#ifndef __MINI_CMD_SERVER_H__
#define __MINI_CMD_SERVER_H__

#include <e32base.h>
#include "SyncServer.pan"

class CMiniCmdServer : public CServer2
{
public:

    static CMiniCmdServer* NewL();
    static CMiniCmdServer* NewLC();

public:
    static TInt ThreadFunction(TAny* aNone);

    void IncrementSessions();
    void DecrementSessions();

protected:
    TInt RunError(TInt aError);

private:
    CMiniCmdServer(TInt aPriority);
    void ConstructL();

private:
    static void PanicClient(const RMessage2& aMessage, TServPanic aReason);
    static void PanicServer(TServPanic aReason);
    static void ThreadFunctionL();

private:
    CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

private:
    TInt iSessionCount;
};

#endif // __MINI_CMD_SERVER_H__
