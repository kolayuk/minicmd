
#ifndef __SERVER_SESSION_H__
#define __SERVER_SESSION_H__

#include <e32base.h>

#include "RequestCmd.h"

static const TUint KDefaultMessageSlots = 0;

// Server UID
const TUid KServerUid3 = { 0xE132DCB4 };

_LIT(KServerFilename, "MiniCMD_Server.EXE");

class RServerSession : public RSessionBase
{
public:
    RServerSession();

public: 
    TInt Connect();
    TVersion Version() const;
    void RequestCommand(TRequestCommand &aCmd) const;

};

#endif  //__SERVER_SESSION_H__
