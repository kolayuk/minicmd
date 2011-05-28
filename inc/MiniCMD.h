/*
 ============================================================================
 Name		    : MiniCMD.h
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
TInt    ParseBatFileL();
void    ParseLineL(TDes &aLine);

//取得命令行中的一个路径
TBool   FindPath(TDes &aLine, TDes &aPath);

// 返回参数所占长度
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
TBool   IsFileExists(const TDesC &aFileName);
void    SetDstAtt(const TDesC &aSrc, const TDesC &aDest, const Parameter &aParam);
//=================================================================================
#endif  // __MINI_CMD_H__
//=================================================================================
///////////////////////////////////////////////////////////////////////////////////
//END OF FILE
///////////////////////////////////////////////////////////////////////////////////
