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
#include <e32svr.h>

#include "MiniCmdSession.h"
#include "Common.h"
#include "FileMan.h"

CMCServerSession* CMCServerSession::NewL(CMiniCmdServer& aServer)
{
    CMCServerSession* self = CMCServerSession::NewLC(aServer);
    CleanupStack::Pop(self);
    return self;
}

CMCServerSession* CMCServerSession::NewLC(CMiniCmdServer& aServer)
{
    CMCServerSession* self = new (ELeave) CMCServerSession(aServer);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}

void CMCServerSession::ConstructL()
{
    iServer.IncrementSessions();
    OpenFileSession();
}

CMCServerSession::CMCServerSession(CMiniCmdServer& aServer)
    : iServer(aServer)
{
}

CMCServerSession::~CMCServerSession()
{
    iServer.DecrementSessions();
    CloseFileSession();
}

void CMCServerSession::ServiceL(const RMessage2& aMessage)
{
    switch (aMessage.Function())
    {
    case EMCServRequestOp :
        RequestCommandL(aMessage);
        break;

    default:
        PanicClient(aMessage, EBadRequest);
        break;
    }
    
    aMessage.Complete(KErrNone);
}

void CMCServerSession::RequestCommandL(const RMessage2& aMessage)
{
    TPckgBuf<TRequestCommand> pkgBuf;
    
    aMessage.Read(0, pkgBuf, 0);
    TRequestCommand request = pkgBuf();
    
    request.iRet = DoCommandL(request.iCmd);
    
    TPtr8 ptr(reinterpret_cast<TUint8*>(&request), sizeof(request), sizeof(request));
    
    aMessage.WriteL(0, ptr, 0);
}

void CMCServerSession::PanicClient(const RMessagePtr2& aMessage, TInt aPanic) const
{
    aMessage.Panic(KMiniCMDServer, aPanic); 
}

TInt CMCServerSession::DoCommandL(const TCommand &aCmd)
{
    switch(aCmd.GetCommand())
    {
    case TCommand::EAtt:
        return SetAtt(aCmd.GetSrc(), aCmd.GetParam());

    case TCommand::ECp:
        return Copy(aCmd.GetSrc(), aCmd.GetDst(), aCmd.GetParam());

    case TCommand::EMv:
        return Move(aCmd.GetSrc(), aCmd.GetDst(), aCmd.GetParam());

    case TCommand::EMd:
        return MkDir(aCmd.GetSrc());

    case TCommand::ERn:
        return Rename(aCmd.GetSrc(), aCmd.GetDst());

    case TCommand::ERm:
        return ::Delete(aCmd.GetSrc(), aCmd.GetParam().is);
        
    case TCommand::EDel:
        return DeleteFile(aCmd.GetSrc(), aCmd.GetParam().is);

    case TCommand::EFile:
        return CreateFile(aCmd.GetSrc());

    default:
        break;
    }

    return KErrNone;
}

///////////////////////////////////////////////////////////////////////////////////
//END OF FILE
///////////////////////////////////////////////////////////////////////////////////
