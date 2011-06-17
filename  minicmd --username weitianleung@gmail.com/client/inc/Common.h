
#ifndef __COMMON_H__
#define __COMMON_H__

#include <e32base.h>

_LIT(KMCServerName,"SyncMCServer" );
_LIT(KMCServerSemaphoreName, "MCServerSemaphore" );

const TUint KMCServMajorVersionNumber = 2;
const TUint KMCServMinorVersionNumber = 0;
const TUint KMCServBuildVersionNumber = 0;

enum TMCServRqst
{
    EMCServRequestOp
};

#endif // __COMMON_H__
