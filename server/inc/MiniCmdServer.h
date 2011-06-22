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
