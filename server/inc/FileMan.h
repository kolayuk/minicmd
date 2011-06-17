// --------------------------------------------------------------------------------
#ifndef __FILE_MAN_H__
#define __FILE_MAN_H__
// --------------------------------------------------------------------------------
#include <e32base.h>

#include "Parameter.h"
// --------------------------------------------------------------------------------
void    OpenFileSession();  //must be call first before any file operation
void    CloseFileSession();

TBool   IsDir(const TDesC &aPath);
void    MakeAtt(TUint &aSet, TUint &aClear, const Parameter &aParam);

TInt    MkDir(const TDesC &aPath);
TInt    SetAtt(const TDesC &aPath, const Parameter &aParam);
TInt    Copy(const TDesC &aSrc, const TDesC &aDest, const Parameter &aParam);
TInt    Rename(const TDesC &aSrc, const TDesC &aDest);
TInt    Delete(const TDesC &aSrc, TBool aIncludeSubdir);
TInt    DeleteFile(const TDesC &aSrc, TBool aIncludeSubdir);    //delete file only
TInt    Move(const TDesC &aSrc, const TDesC &aDest, const Parameter &aParam);

TInt    CreateFile(const TDesC &aFileName);

//TInt    CopyF(const TDesC &aSrc, const TDesC &aDest, TBool aOverwiritten = EFalse);
// --------------------------------------------------------------------------------
#endif  //__FILE_MAN_H__
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////
//END OF FILE
///////////////////////////////////////////////////////////////////////////////////
