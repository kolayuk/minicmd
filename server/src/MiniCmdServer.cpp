/*
 ============================================================================
 Name           : MiniCMD
 Author         : Just Fancy
 Project Home   : http://code.google.com/p/minicmd/
 ============================================================================
    MiniCMD, mini commander for Symbian OS phone
    Copyright (C) 2011  Just Fancy

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ============================================================================
 */
#include <eikstart.h>
#include <e32svr.h>

#include "MiniCmdServer.h"
#include "MiniCmdSession.h"
#include "SyncServer.pan"
#include "Common.h"
// -----------------------------------------------------------------------------
CMiniCmdServer* CMiniCmdServer::NewL()
{
    CMiniCmdServer* server = CMiniCmdServer::NewLC();
    CleanupStack::Pop(server);
    return server;
}
// -----------------------------------------------------------------------------
CMiniCmdServer* CMiniCmdServer::NewLC()
{
    CMiniCmdServer* server = new(ELeave) CMiniCmdServer(EPriorityNormal);
    CleanupStack::PushL(server);
    server->ConstructL();
    return server;
}
// -----------------------------------------------------------------------------
void CMiniCmdServer::ConstructL()
{
    StartL(KMCServerName);
}
// -----------------------------------------------------------------------------
CMiniCmdServer::CMiniCmdServer(TInt aPriority)
    :CServer2(aPriority) 
{
}
// -----------------------------------------------------------------------------
CSession2* CMiniCmdServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
{
    if ( !User::QueryVersionSupported(TVersion(KMCServMajorVersionNumber,
        KMCServMinorVersionNumber,
        KMCServBuildVersionNumber),
        aVersion))
    {
        User::Leave( KErrNotSupported );
    }
    
    return CMCServerSession::NewL(*const_cast<CMiniCmdServer*>(this));
}
// -----------------------------------------------------------------------------
void CMiniCmdServer::IncrementSessions()
{
    iSessionCount++;
}
// -----------------------------------------------------------------------------
void CMiniCmdServer::DecrementSessions()
{
    iSessionCount--;
    if ( iSessionCount <= 0 )
    {
        CActiveScheduler::Stop();
    }
}
// -----------------------------------------------------------------------------
TInt CMiniCmdServer::RunError(TInt aError)
{
    if ( aError == KErrBadDescriptor )
    {
        PanicClient(Message(), EBadDescriptor);
    }
    else
    {
        Message().Complete(aError );
    }

    ReStart();

    return KErrNone;
}
// -----------------------------------------------------------------------------
void CMiniCmdServer::PanicClient(const RMessage2& aMessage, TServPanic aPanic)
{
    aMessage.Panic(KMiniCMDServer, aPanic);
}
// -----------------------------------------------------------------------------
void CMiniCmdServer::PanicServer(TServPanic aPanic)
{
    User::Panic(KMiniCMDServer, aPanic);
}
// -----------------------------------------------------------------------------
void CMiniCmdServer::ThreadFunctionL()
{
    CActiveScheduler* activeScheduler = new(ELeave) CActiveScheduler;
    CleanupStack::PushL(activeScheduler);
    CActiveScheduler::Install(activeScheduler);

    CMiniCmdServer::NewLC();

    RSemaphore semaphore;
    User::LeaveIfError(semaphore.OpenGlobal(KMCServerSemaphoreName));

    semaphore.Signal();
    semaphore.Close();

    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy(2, activeScheduler);
}
// -----------------------------------------------------------------------------
TInt CMiniCmdServer::ThreadFunction(TAny* /*aNone*/)
{
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (!(cleanupStack))
        PanicServer(ECreateTrapCleanup);

    TRAPD(err, ThreadFunctionL());
    if ( err != KErrNone )
    {
        PanicServer(ESrvCreateServer);
    }

    delete cleanupStack;
    cleanupStack = NULL;

    return KErrNone;
}
// -----------------------------------------------------------------------------
TInt E32Main()
{
    return CMiniCmdServer::ThreadFunction(NULL);
}
///////////////////////////////////////////////////////////////////////////////////
//END OF FILE
///////////////////////////////////////////////////////////////////////////////////
