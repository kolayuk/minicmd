// -----------------------------------------------------------------------------
#include <e32math.h>

#include "Common.h"
#include "ServerSession.h"
// -----------------------------------------------------------------------------
static TInt StartServer();
static TInt CreateServerProcess();
// -----------------------------------------------------------------------------
RServerSession::RServerSession()
    : RSessionBase()
{
}
// -----------------------------------------------------------------------------
TInt RServerSession::Connect()
{
    TInt error = ::StartServer();

    if (KErrNone == error)
        error = CreateSession(KMCServerName, Version(), KDefaultMessageSlots);
    
    return error;
}
// -----------------------------------------------------------------------------
TVersion RServerSession::Version() const
{
    return TVersion(KMCServMajorVersionNumber,
        KMCServMinorVersionNumber,
        KMCServBuildVersionNumber);
}
// -----------------------------------------------------------------------------
void RServerSession::RequestCommand(TRequestCommand &aCmd) const
{
    TPtr8 descriptor(reinterpret_cast<TUint8*>(&aCmd), sizeof(aCmd), sizeof(aCmd));
    TIpcArgs args(&descriptor);
    SendReceive(EMCServRequestOp, args);
}
// -----------------------------------------------------------------------------
static TInt StartServer()
{
    TInt result;

    TFindServer findServer(KMCServerName);
    TFullName name;

    result = findServer.Next(name);
    
    if (result == KErrNone)     // Server already running
        return KErrNone;

    RSemaphore semaphore;
    
    result = semaphore.CreateGlobal(KMCServerSemaphoreName, 0);
    if (result != KErrNone)
        return  result;
    
    result = CreateServerProcess();
    if (result != KErrNone)
        return  result;

    semaphore.Wait();
    semaphore.Close();

    return KErrNone;
}
// -----------------------------------------------------------------------------
static TInt CreateServerProcess()
{
    TInt result;

    const TUidType serverUid(KNullUid, KNullUid, KServerUid3);

    RProcess server;

    result = server.Create(KServerFilename, KNullDesC, serverUid);

    if (result != KErrNone)
        return  result;

    server.Resume();
    server.Close();

    return  KErrNone;
}
// -----------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//END OF FILE
////////////////////////////////////////////////////////////////////////////////
