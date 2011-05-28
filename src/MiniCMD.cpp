/*
 ============================================================================
 Name		    : MiniCMD.cpp
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
#include <e32std.h>
#include <f32file.h>
//#include <apgcli.h>
//#include <apacmdln.h>
//#include <BAUTILS.H>
#include <e32math.h>
#include <BADESCA.H>

//#include <e32cons.h>

#include "MiniCMD.h"
//=================================================================================
_LIT(KDefBatFileC, "C:\\Data\\MiniCmd.bat");
_LIT(KDefBatFileE, "E:\\Data\\MiniCmd.bat");
_LIT(KDefBatFileD, "D:\\Data\\MiniCmd.bat");
_LIT(KDefBatFileZ, "Z:\\Data\\MiniCmd.bat");

_LIT(KDisableFile, "MiniCmd.disable");

RFs iFs;
CFileMan *fileMan = NULL;
CArrayFixFlat<TCommand> *cmdArray = NULL;
//for debug only
//print information is more comfortable
//than write log file 
//LOCAL_D CConsoleBase* console = NULL;
//=================================================================================
LOCAL_C void MainL()
{
    /*    TRAPD(createError, console = Console::NewL(_L("Console"), TSize(KConsFullScreen, KConsFullScreen)));
    if (createError){
        return;
    }
     */
    User::LeaveIfError(iFs.Connect());
    fileMan = CFileMan::NewL(iFs);
    if (!fileMan)
        return ;

    cmdArray = new(ELeave) CArrayFixFlat<TCommand>(10);
    CleanupStack::PushL(cmdArray);

    if (!IsCmdDisabled())
        ParseBatFileL();

    for(TInt i=0; i<cmdArray->Count(); i++)
    {
        TCommand cmd = cmdArray->operator [](i);
        if (cmd.AbortIfExists())
        {
            if (IsFileExists(cmd.GetSrc()))
                break;
        }
        else if(cmd.AbortIfNotExists())
        {
            if (!IsFileExists(cmd.GetSrc()))
                break;
        }
        else if (cmd.SkipIfExists())
        {
            if (IsFileExists(cmd.GetSrc()))
            {
                i++;
                continue;
            }
        }
        else if (cmd.SkipIfNotExists())
        {
            if (!IsFileExists(cmd.GetSrc()))
            {
                i++;
                continue;
            }
        }
        else if(cmd.BackIfExists())
        {
            if (IsFileExists(cmd.GetSrc()))
            {
                if (i > 0)  //should not be the first cmd,
                {           //otherwise, MiniCMD will crash...
                    i -= 2;
                    continue;
                }
            }
        }
        else if(cmd.BackIfNotExists())
        {
            if (!IsFileExists(cmd.GetSrc()))
            {
                if (i > 0)
                {
                    i -= 2;
                    continue;
                }
            }
        }
        else if (cmd.RestartIfExists())
        {
            if (IsFileExists(cmd.GetSrc()))
            {
                if (i > 0)
                {
                    i = -1;
                    continue;
                }
            }
        }
        else if(cmd.RestartIfNotExists())
        {
            if (!IsFileExists(cmd.GetSrc()))
            {
                if (i > 0)
                {
                    i = -1;
                    continue;
                }
            }
        }

        DoCommand(cmd);
    }

    CleanupStack::PopAndDestroy(cmdArray);
    delete fileMan;
    iFs.Close();

    //    delete console;
}
//=================================================================================
GLDEF_C TInt E32Main()
{     
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();

    TRAPD(iErr, MainL());

    delete cleanup;

    __UHEAP_MARKEND;

    return KErrNone;
}
//=================================================================================
TInt MkDir(const TDesC &aPath)
{
    return iFs.MkDirAll(aPath);
}
//=================================================================================
TInt ParseBatFileL()
{
    RFile file;
    TInt errCode = KErrNone;

    if (KErrNone != (errCode = file.Open(iFs, KDefBatFileC, EFileShareReadersOnly | EFileRead)))
        if (KErrNone != (errCode = file.Open(iFs, KDefBatFileE, EFileShareReadersOnly | EFileRead)))
            if (KErrNone != (errCode = file.Open(iFs, KDefBatFileD, EFileShareReadersOnly | EFileRead)))
                if (KErrNone != (errCode = file.Open(iFs, KDefBatFileZ, EFileShareReadersOnly | EFileRead)))
                    return errCode;

    if (errCode != KErrNone)
        return errCode;

    CleanupClosePushL(file);

    TFileText fileText;
    fileText.Set(file);

    //one line should not exceed 512 bytes
    HBufC* heapBuf = HBufC::NewLC(512);
    TPtr lineBuf = heapBuf->Des();

    while (true)
    {
        if (fileText.Read(lineBuf) == KErrEof)
            break;      

        ParseLineL(lineBuf);
    }

    CleanupStack::PopAndDestroy(heapBuf);
    CleanupStack::PopAndDestroy(&file);

    if (KErrEof == errCode)
        errCode = KErrNone;

    return errCode;
}
//=================================================================================
void ParseLineL(TDes &aLine)
{
    TBuf<24> cmd;

    if (!GetCMD(aLine, cmd))
        return ;

    cmd.Trim();
    cmd.LowerCase();    //case insensitive

    aLine.Delete(0, cmd.Length());
    aLine.TrimLeft();

    Parameter iParam;
    TFileName iSrc, iDest;

    ParseCMD(aLine, iSrc, &iDest, &iParam);
    iSrc.Trim();
    iDest.Trim();

    if (iSrc.Length() <= 0)  //currently, src will never be empty
        return ;

    if (cmd == _L("att"))
    {    
        if (iParam.NeedToSetAtt())
            cmdArray->AppendL(TCommand(TCommand::EAtt, &iParam, &iSrc));
    }
    else if(cmd == _L("cp") || cmd == _L("copy"))
    {
        if (iDest.Length() > 0)
            cmdArray->AppendL(TCommand(TCommand::ECp, &iParam, &iSrc, &iDest));
    }
    else if(cmd == _L("mv"))
    {
        if (iDest.Length() > 0)
            cmdArray->AppendL(TCommand(TCommand::EMv, &iParam, &iSrc, &iDest));
    }
    else if(cmd == _L("md") || cmd == _L("mkdir"))
    {
        if (iSrc[iSrc.Length() - 1] != '\\')
            iSrc.Append('\\');

        cmdArray->AppendL(TCommand(TCommand::EMd, NULL, &iSrc));
    }
    else if(cmd == _L("rn") || cmd == _L("rename"))
    {
        if (iDest.Length() > 0)
            cmdArray->AppendL(TCommand(TCommand::ERn, NULL, &iSrc, &iDest));
    }
    else if(cmd == _L("rm") || cmd == _L("rmdir"))
    {
        cmdArray->AppendL(TCommand(TCommand::ERm, &iParam, &iSrc));
    }
    else if (cmd == _L("delete"))
    {
        cmdArray->AppendL(TCommand(TCommand::ERm, &iParam, &iSrc));
    }
    else if(cmd == _L("abort_if_exists"))
    {
        cmdArray->AppendL(TCommand(TCommand::EDel, NULL, &iSrc));
    }
    else if(cmd == _L("abort_if_not_exists"))
    {
        cmdArray->AppendL(TCommand(TCommand::EAbnes, NULL, &iSrc));
    }
    else if(cmd == _L("skip_if_exists"))
    {
        cmdArray->AppendL(TCommand(TCommand::ESkes, NULL, &iSrc));
    }
    else if(cmd == _L("skip_if_not_exists"))
    {
        cmdArray->AppendL(TCommand(TCommand::ESknes, NULL, &iSrc));
    }
    else if(cmd == _L("back_if_exists"))
    {
        cmdArray->AppendL(TCommand(TCommand::EBkes, NULL, &iSrc));
    }
    else if(cmd == _L("back_if_not_exists"))
    {
        cmdArray->AppendL(TCommand(TCommand::EBknes, NULL, &iSrc));
    }
    else if(cmd == _L("restart_if_exists"))
    {
        cmdArray->AppendL(TCommand(TCommand::ERses, NULL, &iSrc));
    }
    else if(cmd == _L("restart_if_not_exists"))
    {
        cmdArray->AppendL(TCommand(TCommand::ERsnes, NULL, &iSrc));
    }
}
//=================================================================================
TBool FindPath(TDes &aLine, TDes &aPath)
{
    aLine.TrimLeft();

    if (aLine.Length() < 2)
        return EFalse;

    if(aLine[0] == '"') //以引号开始的
    {
        int i=0;
        while(++i<aLine.Length() && aLine[i] != '"')
            aPath.Append(aLine[i]);

        return i>=1;
    }
    else
    {
        int i=-1;

        while(++i<aLine.Length() && aLine[i] != ' ')    //以空格结束
            aPath.Append(aLine[i]);

        return i>=1;
    }

    return EFalse;
}
//=================================================================================
TInt GetParams(TDes &aLine, Parameter &aParam)
{
    aLine.TrimLeft();

    if (aLine.Length() < 2)
        return 0;

    int i = -1;

    AttStatus status = not_set;
    TBool bSlash = EFalse;  //is '/'

    while(++i < aLine.Length())
    {
        if (aLine[i] == ' ')    //skip the space
            continue;

        if ((aLine[i] == '-' || aLine[i] == '+' || aLine[i] == '/') && i != aLine.Length() - 1)
        {
            if (i + 2 < aLine.Length())
                if (aLine[i+2] != ' ')  //参数后必须有空格
                    return i;

            bSlash = EFalse;

            if (aLine[i] == '+')
                status = add_to;
            else if (aLine[i] == '-')
                status = rm_it;
            else
                bSlash = ETrue;

            if (++i >= aLine.Length())
                break;

            //case insensitive
            if (aLine[i] == 's' || aLine[i] == 'S')
            {
                if (bSlash)
                    aParam.is = ETrue;
                else
                    aParam.s = status;
            }
            else if (aLine[i] == 'h' || aLine[i] == 'H')
                aParam.h = status;
            else if (aLine[i] == 'r' || aLine[i] == 'R')
                aParam.r = status;
            else if(aLine[i] == 'o' || aLine[i] == 'O')
                aParam.ow = ETrue;  // /o or -o is OK

        }
        else    //规定参数紧跟命令后
            return i;
    }

    return 0;
}
//=================================================================================
TBool IsDir(const TDesC &aPath)
{
    TParse iParse;
    iFs.Parse(aPath, iParse);

    if (iParse.IsWild())    //有通配符的当文件处理
        return EFalse;

    if (aPath[aPath.Length() - 1] == '\\')
        return ETrue;

    TUint iAtt = KEntryAttNormal;
    iFs.Att(aPath, iAtt);
    if (iAtt & KEntryAttDir)
        return ETrue;

    return EFalse;
}
//=================================================================================
TInt SetAtt(const TDesC &aPath, const Parameter &aParam)
{
    TUint iSetAttMask = KEntryAttNormal;
    TUint iClearAttMask = KEntryAttNormal;

    //if no attributes needs to change
    //just return back
    if (!aParam.NeedToSetAtt())
        return -1;

    if (aParam.s == add_to)
        iSetAttMask |= KEntryAttSystem;
    if (aParam.h == add_to)
        iSetAttMask |= KEntryAttHidden;
    if (aParam.r == add_to)
        iSetAttMask |= KEntryAttReadOnly;

    if (aParam.s == rm_it)
        iClearAttMask |= KEntryAttSystem;
    if (aParam.h == rm_it)
        iClearAttMask |= KEntryAttHidden;
    if (aParam.r == rm_it)
        iClearAttMask |= KEntryAttReadOnly;

    TInt ret = KErrNone;
    //use synchronous
    if (aParam.is)
    { 
        ret = fileMan->Attribs(aPath, iSetAttMask, iClearAttMask, 0, CFileMan::ERecurse);
        if (aPath[aPath.Length() - 1] != '\\')
        {
            TFileName tmp;
            tmp.Copy(aPath);
            tmp.Append('\\');

            ret = fileMan->Attribs(tmp, iSetAttMask, iClearAttMask, 0, CFileMan::ERecurse);
        }
    }
    else
    {
        ret = fileMan->Attribs(aPath, iSetAttMask, iClearAttMask, 0);
    }

    return ret;
}
//=================================================================================
TInt Copy(const TDesC &aSrc, const TDesC &aDest, const Parameter &aParam)
{         
    TInt ret = KErrNone;
    if (IsDir(aSrc))
    {
        ret = fileMan->Copy(aSrc, aDest, aParam.ow ? CFileMan::EOverWrite | CFileMan::ERecurse :
        CFileMan::ERecurse);
    }
    else
    {
        if (!aParam.is)
        {
            TParse iParse;
            iFs.Parse(aDest, iParse);
            MkDir(iParse.DriveAndPath());   //ensure the folder exists
        }

        ret = fileMan->Copy(aSrc, aDest, aParam.is ? (
            aParam.ow ? CFileMan::EOverWrite | CFileMan::ERecurse : CFileMan::ERecurse)
            : ( aParam.ow ? CFileMan::EOverWrite : 0));
    }

    if (aParam.NeedToSetAtt())
        SetDstAtt(aSrc, aDest, aParam);

    return ret;
}
//=================================================================================
TInt Rename(const TDesC &aSrc, const TDesC &aDest)
{
    return fileMan->Rename(aSrc, aDest, 0); //no overwritten exists name
}
//=================================================================================
TInt Delete(const TDesC &aSrc, TBool aIncludeSubdir)
{
    if (IsDir(aSrc))
    {
        TFileName buf;
        buf.Copy(aSrc);

        //如果最后的字符不是'\'
        if (aSrc[aSrc.Length()-1] != '\\'){
            buf.Append('\\');
        }

        Parameter iParam;
        iParam.r = rm_it;
        iParam.is = ETrue;
        SetAtt(aSrc, iParam);   //去除只读属性，确保能删除

        return fileMan->RmDir(buf);
    }
    else
    {
        return DeleteFile(aSrc, aIncludeSubdir);
    }
}
//=================================================================================
TInt DeleteFile(const TDesC &aSrc, TBool aIncludeSubdir)
{
    Parameter iParam;
    iParam.r = rm_it;
    iParam.is = aIncludeSubdir;
    SetAtt(aSrc, iParam);

    return fileMan->Delete(aSrc, aIncludeSubdir ? CFileMan::ERecurse : 0);
}
//=================================================================================
TInt Move(const TDesC &aSrc, const TDesC &aDest, const Parameter &aParam)
{
    TInt ret = KErrNone;
    if (IsDir(aSrc))
    {
        ret = fileMan->Move(aSrc, aDest, aParam.ow ? CFileMan::ERecurse | CFileMan::EOverWrite : CFileMan::ERecurse);
    }
    else
    {
        if (!aParam.is)
        {
            TParse iParse;
            iFs.Parse(aDest, iParse);
            MkDir(iParse.DriveAndPath());   //ensure the folder exists
        }

        if (aParam.ow)
            ret = fileMan->Move(aSrc, aDest, aParam.is ? CFileMan::ERecurse | CFileMan::EOverWrite : CFileMan::EOverWrite);
        else
            ret = fileMan->Move(aSrc, aDest, aParam.is ? CFileMan::ERecurse : 0);
    }

    if (aParam.NeedToSetAtt())
        SetDstAtt(aSrc, aDest, aParam);

    return ret;
}
//=================================================================================
TBool GetCMD(TDes &aLine, TDes &cmd)
{
    aLine.Trim();

    //empty line
    if (aLine.Length() == 0)
        return EFalse;

    //comment line
    if (aLine[0] == ';' || aLine[0] == '#')
        return EFalse;

    TInt posSpace = aLine.Locate(' ');

    //invalid command
    if (posSpace == KErrNotFound ||
        posSpace > 24 || posSpace == 0 ||
        posSpace == aLine.Length() - 1
    )
        return EFalse;

    cmd.Copy(aLine.Mid(0, posSpace));

    return ETrue;
}
//=================================================================================
void ParseCMD(TDes &aCMD, TDes &aSrc, TDes *aDest/* = NULL*/, Parameter *aParam/* = NULL*/)
{
    //parse first
    if (aParam != NULL)
    {
        TInt len = GetParams(aCMD, *aParam);
        if (len > 0)
        {
            aCMD.Delete(0, len);
            aCMD.TrimLeft();
        }
    }

    //then source path
    if (!FindPath(aCMD, aSrc))
        return ;

    if (aDest != NULL)
    {
        if (aSrc.Length() < 0)
            return ;
        //Important to +2 when the first path enclosed by quote
        if (aCMD[0] == '"')
            aCMD.Delete(0, aSrc.Length() + 2);
        else
            aCMD.Delete(0, aSrc.Length());

        aCMD.TrimLeft();
        FindPath(aCMD, *aDest);
    }
}
//=================================================================================
TBool IsCmdDisabled()
{
    TFindFile findFile(iFs);
    //Search in drive from Y to A, and the last is Z
    if (KErrNone == findFile.FindByDir(KDisableFile, _L("\\")))
        return ETrue;

    return EFalse;
}
//=================================================================================
TInt DoCommand(const TCommand &aCmd)
{
    TInt ret = KErrNone;

    switch(aCmd.GetCommand())
    {
    case TCommand::EAtt:
        ret = SetAtt(aCmd.GetSrc(), aCmd.GetParam());
        break;

    case TCommand::ECp:
        ret = Copy(aCmd.GetSrc(), aCmd.GetDst(), aCmd.GetParam());
        break;

    case TCommand::EMv:
        ret = Move(aCmd.GetSrc(), aCmd.GetDst(), aCmd.GetParam());
        break;

    case TCommand::EMd:
        ret = MkDir(aCmd.GetSrc());
        break;

    case TCommand::ERn:
        ret = Rename(aCmd.GetSrc(), aCmd.GetDst());
        break;

    case TCommand::ERm:
        ret = Delete(aCmd.GetSrc(), aCmd.GetParam().is);
        break;

    case TCommand::EDel:
        ret = DeleteFile(aCmd.GetSrc(), aCmd.GetParam().is);
        break;

    default:
        ret = KErrNotFound;
    }

    return ret;
}
//=================================================================================
TBool IsFileExists(const TDesC &aFileName)
{
    RFile file;

    TInt code = file.Open(iFs, aFileName, EFileRead);
    file.Close();

    if (code == KErrNone)
        return ETrue;

    return EFalse;
}
//=================================================================================
void SetDstAtt(const TDesC &aSrc, const TDesC &aDest, const Parameter &aParam)
{
    if (IsDir(aDest))
    {
        if (IsDir(aSrc))
        {
            Parameter p = aParam;
            p.is = ETrue;
            SetAtt(aDest, p);
        }
        else    //
        {
            TFileName tmp;
            tmp.Copy(aDest);

            if (tmp[tmp.Length() - 1] != '\\')
                tmp.Append('\\');

            TInt pos = aSrc.LocateReverse('\\');

            if (pos != KErrNotFound){
                tmp.Append(aSrc.Right(aSrc.Length() - pos - 1));
            }else{
                tmp.Append(aSrc);
            }

            SetAtt(tmp, aParam);
        }
    }
    else      //destination
    {
        SetAtt(aDest, aParam);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////
//END OF FILE
///////////////////////////////////////////////////////////////////////////////////////////////
