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
