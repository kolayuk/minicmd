/*
 ============================================================================
 Name		    : MiniCMD
 Author	        : Just Fancy
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
#ifndef __MINI_CMD_H__
#define __MINI_CMD_H__
//=================================================================================
#include <e32base.h>

#include "TCommand.h"
//=================================================================================
TInt    LoadCmdFileL(const TDesC &aFileName);
void    ParseLineL(TDes &aLine);
TBool   FindPath(TDes &aLine, TDes &aPath);
TInt    GetParams(TDes &aLine, Parameter &aParam);
TInt    MkDir(const TDesC &aPath);
TBool   IsDir(const TDesC &aPath);
TInt    SetAtt(const TDesC &aPath, const Parameter &aParam);
TInt    Copy(const TDesC &aSrc, const TDesC &aDest, const Parameter &aParam);
TInt    Rename(const TDesC &aSrc, const TDesC &aDest);
TInt    Delete(const TDesC &aSrc, TBool aIncludeSubdir);
TInt    DeleteFile(const TDesC &aSrc, TBool aIncludeSubdir);    //delete file only
TInt    Move(const TDesC &aSrc, const TDesC &aDest, const Parameter &aParam);
TBool   GetCMD(TDes &aLine, TDes &cmd);
void    ParseCMD(TDes &aCMD, TDes &aSrc, TDes *aDest = NULL, Parameter *aParam = NULL);
TInt    DoCommand(const TCommand &aCmd);
TBool   IsCmdDisabled();    //test if usr don't want to run minicmd
TBool   IsPathFileExists(const TDesC &aPath);
void    Run(const CArrayFixFlat<TCommand> &aCmdSet);
void    MakeAtt(TUint &aSet, TUint &aClear, const Parameter &aParam);
inline  void SkipCmd(const CArrayFixFlat<TCommand> &aCmdSet, TInt &i);
TInt    RunApp(const TDesC &aName);
TInt    KillApp(TInt aUid);
TInt    KillApp(const TDesC &aName);
TInt    DoKill(const TDesC &aStr);
inline  void Sleep(TInt aMs);
TInt32  HexStr2Int32(const TDesC & aHexStr);
TInt32  DecStr2Int32(const TDesC &aDecStr);
//=================================================================================
#endif  // __MINI_CMD_H__
//=================================================================================
///////////////////////////////////////////////////////////////////////////////////
//END OF FILE
///////////////////////////////////////////////////////////////////////////////////
