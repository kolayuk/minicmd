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
// --------------------------------------------------------------------------------
#endif  //__FILE_MAN_H__
// --------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////
//END OF FILE
///////////////////////////////////////////////////////////////////////////////////
