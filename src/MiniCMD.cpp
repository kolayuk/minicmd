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
#include <BADESCA.H>
#include <e32math.h>

#include "MiniCMD.h"
#include "TFileMonitor.h"
#include "Constant.h"
//=================================================================================
RFs iFs;
CFileMan *fileMan = NULL;
CArrayFixFlat<TCommand> *iCmdSet = NULL;
TFileMonitor *fileMonitor = NULL;
//=================================================================================
//to disable log file, comment the below line
//#define _USING_LOG_FILE_
//=================================================================================
#ifdef _USING_LOG_FILE_
#include "MiniLog.h"

CMiniLog *miniLog = NULL;
#endif
//=================================================================================
void LogFile(TRefByValue<const TDesC16> aFmt, ...)
{
#ifdef _USING_LOG_FILE_
    VA_LIST ap;
    VA_START(ap, aFmt);

    TBuf16<512> iBuf;

    iBuf.AppendFormatList(aFmt, ap);

    VA_END(ap);
    miniLog->Log(iBuf);
#endif
}
//=================================================================================
LOCAL_C void MainL()
{ 
    User::LeaveIfError(iFs.Connect());
    
    fileMan = CFileMan::NewL(iFs);
    if (!fileMan)
        return ;
    CleanupStack::PushL(fileMan);
    
    iCmdSet = new(ELeave) CArrayFixFlat<TCommand>(10);
    if (!iCmdSet)
    {
        CleanupStack::PopAndDestroy();
        return ;
    }
    CleanupStack::PushL(iCmdSet);

#ifdef _USING_LOG_FILE_
    miniLog = CMiniLog::NewL(iFs);
    if (!miniLog)
    {
        CleanupStack::PopAndDestroy(iCmdSet);
        CleanupStack::PopAndDestroy(fileMan);
        
        return ;
    }
    CleanupStack::PushL(miniLog);

    miniLog->Log(_L("[Starting MiniCMD]"));
#endif
    
    fileMonitor = new TFileMonitor(*fileMan);
    fileMan->SetObserver(fileMonitor);
    
    if (!IsCmdDisabled())
    {
        if (IsFileExists(KDefCMDFileD))
            LoadCmdFileL(KDefCMDFileD);
        else if (IsFileExists(KDefCMDFileE))
            LoadCmdFileL(KDefCMDFileE);
        else if (IsFileExists(KDefCMDFileC))
            LoadCmdFileL(KDefCMDFileC);
        else if (IsFileExists(KDefCMDFileZ))
            LoadCmdFileL(KDefCMDFileZ);
#ifdef _USING_LOG_FILE_
        else
            LogFile(_L("No MiniCmd.txt"));
#endif
        
        Run(*iCmdSet);
    }
#ifdef _USING_LOG_FILE_
    else
        LogFile(_L("[MiniCMD disabled]"));
#endif
    
    LogFile(_L("[Exiting MiniCMD normally]"));
    

#ifdef _USING_LOG_FILE_
    CleanupStack::PopAndDestroy(miniLog);
#endif
    CleanupStack::PopAndDestroy(iCmdSet);
    CleanupStack::PopAndDestroy(fileMan);
    delete fileMonitor;
    iFs.Close();
}
//=================================================================================
LOCAL_C void DoStartL()
{
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    MainL();

    CleanupStack::PopAndDestroy(scheduler);
}
//=================================================================================
GLDEF_C TInt E32Main()
{     
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    
    TRAPD(iErr, DoStartL());
    
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
TInt LoadCmdFileL(const TDesC &aFileName)
{
    RFile file;
    TInt errCode = KErrNone;
    
    errCode = file.Open(iFs, aFileName, EFileShareReadersOnly | EFileRead);

#ifdef _USING_LOG_FILE_
    TBuf<512> logBuf;
    logBuf.Append(_L("Load ["));
    logBuf.Append(aFileName);
    logBuf.Append(_L("]"));
 
    miniLog->Log(logBuf);
    miniLog->Log(_L("ret [%d]"), errCode);
#endif

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

    if (cmd == KCmdAtt)
    {    
        if (iParam.NeedToSetAtt() && iSrc.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::EAtt, &iParam, &iSrc));
    }
    else if(cmd == KCmdCp || cmd == KCmdCopy)
    {
        if (iDest.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::ECp, &iParam, &iSrc, &iDest));
    }
    else if(cmd == KCmdMv)
    {
        if (iDest.Length() > 0  && iSrc.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::EMv, &iParam, &iSrc, &iDest));
    }
    else if(cmd == KCmdMd || cmd == KCmdMkDir)
    {
        if (iSrc.Length() > 0 && iSrc[iSrc.Length() - 1] != '\\')
            iSrc.Append('\\');

        iCmdSet->AppendL(TCommand(TCommand::EMd, NULL, &iSrc));
    }
    else if(cmd == KCmdRn || cmd == KCmdRename)
    {
        if (iSrc.Length() > 0 && iDest.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::ERn, NULL, &iSrc, &iDest));
    }
    else if(cmd == KCmdRm || cmd == KCmdRmDir)
    {
        if (iSrc.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::ERm, &iParam, &iSrc));
    }
    else if (cmd == KCmdDel)
    {
        if (iSrc.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::EDel, &iParam, &iSrc));
    }
    else if (cmd == KCmdRun)
    {
        if (iSrc.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::ERun, NULL, &iSrc));
    }
    else if(cmd == KCmdKill)
    {
        if (iSrc.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::EKill, NULL, &iSrc));
    }
    else if(cmd == KCmdSleep)
    {
        if (iSrc.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::ESleep, NULL, &iSrc));
    }
    else if(cmd == KCmdAbes)
    {
        if (iSrc.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::EAbes, NULL, &iSrc));
    }
    else if(cmd == KCmdAbnes)
    {
        if (iSrc.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::EAbnes, NULL, &iSrc));
    }
    else if(cmd == KCmdSkes)
    {
        if (iSrc.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::ESkes, NULL, &iSrc));
    }
    else if(cmd == KCmdSknes)
    {
        if (iSrc.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::ESknes, NULL, &iSrc));
    }
    else if(cmd == KCmdBkes)
    {
        if (iSrc.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::EBkes, NULL, &iSrc));
    }
    else if(cmd == KCmdBknes)
    {
        if (iSrc.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::EBknes, NULL, &iSrc));
    }
    else if(cmd == KCmdRses)
    {
        if (iSrc.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::ERses, NULL, &iSrc));
    }
    else if(cmd == KCmdRsnes)
    {
        if (iSrc.Length() > 0)
            iCmdSet->AppendL(TCommand(TCommand::ERsnes, NULL, &iSrc));
    }
    /*  else if (cmd == _L("cmd"))
    {
        LoadCmdFileL(iSrc);
    }
     */   else if(cmd == KCmdIf)
     {
         if (iSrc.Length() > 0)
             iCmdSet->AppendL(TCommand(TCommand::EIf, NULL, &iSrc, iDest.Length() > 0 ? &iDest : NULL));
     }
     else if(cmd == KCmdIfn)
     {
         if (iSrc.Length() > 0)
             iCmdSet->AppendL(TCommand(TCommand::EIfn, NULL, &iSrc, iDest.Length() > 0 ? &iDest : NULL));
     }
     else if(cmd == KCmdElse)
     {
         iCmdSet->AppendL(TCommand(TCommand::EElse, NULL, NULL));
     }
     else if(cmd == KCmdElseIf)
     {
         if (iSrc.Length() > 0)
             iCmdSet->AppendL(TCommand(TCommand::EElseIf, NULL, &iSrc, iDest.Length() > 0 ? &iDest : NULL));
     }
     else if(cmd == KCmdElseIfn)
     {
         if (iSrc.Length() > 0)
             iCmdSet->AppendL(TCommand(TCommand::EElseIfn, NULL, &iSrc, iDest.Length() > 0 ? &iDest : NULL));
     }
     else if(cmd == KCmdEndIf)
     {
         iCmdSet->AppendL(TCommand(TCommand::EEndIf, NULL, NULL));
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
    
    MakeAtt(iSetAttMask, iClearAttMask, aParam);

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
    
    if (aParam.NeedToSetAtt())
    {
        TUint iSetAttMask = KEntryAttNormal;
        TUint iClearAttMask = KEntryAttNormal;
        
        MakeAtt(iSetAttMask, iClearAttMask, aParam);
        fileMonitor->SetMode(TFileMonitor::ECopy);
        fileMonitor->SetAtt(iSetAttMask, iClearAttMask);
    }

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
    
    fileMonitor->SetMode(TFileMonitor::ENone);
    
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
    TInt ret = KErrNone;

    if (IsDir(aSrc))
    {
        TFileName buf;
        buf.Copy(aSrc);

        //如果最后的字符不是'\'
        if (aSrc[aSrc.Length()-1] != '\\'){
            buf.Append('\\');
        }
        
        fileMonitor->SetMode(TFileMonitor::EDelete);
        
        ret = fileMan->RmDir(buf);
    }
    else
    {
        ret = DeleteFile(aSrc, aIncludeSubdir);
    }
    
    fileMonitor->SetMode(TFileMonitor::ENone);
    
    return ret;
}
//=================================================================================
TInt DeleteFile(const TDesC &aSrc, TBool aIncludeSubdir)
{
    TInt ret = KErrNone;
    
    fileMonitor->SetMode(TFileMonitor::EDelete);
    
    ret = fileMan->Delete(aSrc, aIncludeSubdir ? CFileMan::ERecurse : 0);
    
    fileMonitor->SetMode(TFileMonitor::ENone);
    
    return ret;
}
//=================================================================================
TInt Move(const TDesC &aSrc, const TDesC &aDest, const Parameter &aParam)
{
    TInt ret = KErrNone;
    
    if (aParam.NeedToSetAtt())
    {
        TUint iSetAttMask = KEntryAttNormal;
        TUint iClearAttMask = KEntryAttNormal;

        MakeAtt(iSetAttMask, iClearAttMask, aParam);
        fileMonitor->SetMode(TFileMonitor::ECopy);
        fileMonitor->SetAtt(iSetAttMask, iClearAttMask);
    }
    
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

    fileMonitor->SetMode(TFileMonitor::ENone);
    
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

    if (posSpace == KErrNotFound && aLine.Length() <= 24)   //
    {
        cmd.Copy(aLine);
        return ETrue;
    }

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
    
#ifdef _USING_LOG_FILE_
    TBuf<512> logBuf;
#endif

    switch(aCmd.GetCommand())
    {
    case TCommand::EAtt:
#ifdef _USING_LOG_FILE_
        logBuf.Append(_L("att ["));
        logBuf.Append(aCmd.GetSrc());
        logBuf.Append(_L("]"));
        miniLog->Log(logBuf);
#endif
        ret = SetAtt(aCmd.GetSrc(), aCmd.GetParam());
        break;

    case TCommand::ECp:
#ifdef _USING_LOG_FILE_
        logBuf.Append(_L("cp ["));
        logBuf.Append(aCmd.GetSrc());
        logBuf.Append(_L("] ["));
        logBuf.Append(aCmd.GetDst());
        logBuf.Append(_L("]"));
        miniLog->Log(logBuf);
#endif
        ret = Copy(aCmd.GetSrc(), aCmd.GetDst(), aCmd.GetParam());
        break;

    case TCommand::EMv:
#ifdef _USING_LOG_FILE_
        logBuf.Append(_L("mv ["));
        logBuf.Append(aCmd.GetSrc());
        logBuf.Append(_L("] ["));
        logBuf.Append(aCmd.GetDst());
        logBuf.Append(_L("]"));
        miniLog->Log(logBuf);
#endif
        ret = Move(aCmd.GetSrc(), aCmd.GetDst(), aCmd.GetParam());
        break;

    case TCommand::EMd:
#ifdef _USING_LOG_FILE_
        logBuf.Append(_L("md ["));
        logBuf.Append(aCmd.GetSrc());
        logBuf.Append(_L("]"));
        miniLog->Log(logBuf);
#endif
        ret = MkDir(aCmd.GetSrc());
        break;

    case TCommand::ERn:
#ifdef _USING_LOG_FILE_
        logBuf.Append(_L("rn ["));
        logBuf.Append(aCmd.GetSrc());
        logBuf.Append(_L("] ["));
        logBuf.Append(aCmd.GetDst());
        logBuf.Append(_L("]"));
        miniLog->Log(logBuf);
#endif
        ret = Rename(aCmd.GetSrc(), aCmd.GetDst());
        break;

    case TCommand::ERm:
#ifdef _USING_LOG_FILE_
        logBuf.Append(_L("rm ["));
        logBuf.Append(aCmd.GetSrc());
        logBuf.Append(_L("]"));
        miniLog->Log(logBuf);
#endif
        ret = Delete(aCmd.GetSrc(), aCmd.GetParam().is);
        break;

    case TCommand::EDel:
#ifdef _USING_LOG_FILE_
        logBuf.Append(_L("delete ["));
        logBuf.Append(aCmd.GetSrc());
        logBuf.Append(_L("]"));
        miniLog->Log(logBuf);
#endif
        ret = DeleteFile(aCmd.GetSrc(), aCmd.GetParam().is);
        break;
        
    case TCommand::ERun:
#ifdef _USING_LOG_FILE_
        logBuf.Append(_L("run ["));
        logBuf.Append(aCmd.GetSrc());
        logBuf.Append(_L("]"));
        miniLog->Log(logBuf);
#endif
        ret = RunApp(aCmd.GetSrc());
        break;
        
    case TCommand::EKill:
#ifdef _USING_LOG_FILE_
        logBuf.Append(_L("kill ["));
        logBuf.Append(aCmd.GetSrc());
        logBuf.Append(_L("]"));
        miniLog->Log(logBuf);
#endif
        ret = DoKill(aCmd.GetSrc());
        break;
        
    case TCommand::ESleep:
#ifdef _USING_LOG_FILE_
        logBuf.Append(_L("sleep ["));
        logBuf.Append(aCmd.GetSrc());
        logBuf.Append(_L("]"));
        miniLog->Log(logBuf);
#endif
        Sleep(DecStr2Int32(aCmd.GetSrc()));
        break;

    default:
#ifdef _USING_LOG_FILE_
        miniLog->Log(_L("Not define cmd"));
#endif
        ret = KErrNotFound;
    }
    
#ifdef _USING_LOG_FILE_
    miniLog->Log(_L("ret [%d]"), ret);
#endif

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
TBool IsFolderExists(const TDesC &aFolder)
{
    if (!IsDir(aFolder))
        return EFalse;
    
    TFileName temp;
    temp.Copy(aFolder);
    
    if (temp[temp.Length() - 1] != '\\')
        temp.Append('\\');
    
    TInt ret = iFs.MkDir(temp);
    if (ret == KErrAlreadyExists)
        return ETrue;
    
    if (ret == KErrNone)
    {
        fileMan->RmDir(temp);
        return ETrue;
    }

    return EFalse;
}
//=================================================================================
TBool IsPathExists(const TDesC &aPath)
{
    return IsFileExists(aPath) ||
        IsFolderExists(aPath);
}
//=================================================================================
void Run(const CArrayFixFlat<TCommand> &aCmdSet)
{
    TBool fLastCondition = EFalse;

    for(TInt i=0; i<aCmdSet.Count(); i++)
    {
        TCommand cmd = aCmdSet.operator [](i);
        TCommand::TCommandSet cs = cmd.GetCommand();

        if (cmd.AbortIfExists() || cmd.AbortIfNotExists())  //abort_if_(not_)exists
        {
            TBool fExists = IsPathExists(cmd.GetSrc());
            if (cmd.AbortIfExists() && fExists)
                break;
            
            if (cmd.AbortIfNotExists() && !fExists)
                break;
        }
        else if (cmd.SkipIfExists() || cmd.SkipIfNotExists())   //skip_if_(not_)exists
        {
            TBool fExists = IsPathExists(cmd.GetSrc());
            
            if ((cmd.SkipIfExists() && fExists) || (cmd.SkipIfNotExists() && !fExists))
                i++;

            continue;
        }
        else if(cmd.BackIfExists() || cmd.BackIfNotExists())    //back_if_(not_)exists
        {
            TBool fExists = IsPathExists(cmd.GetSrc());
            
            if ((cmd.BackIfExists() && fExists) || (cmd.BackIfNotExists() && !fExists))
            {
                if (i > 0)  //should not be the first cmd,
                    i -= 2;
            }
            continue;
        }
        else if (cmd.RestartIfExists() || cmd.RestartIfNotExists()) //restart_if_(not_)exists
        {
            TBool fExists = IsPathExists(cmd.GetSrc());
            if ((cmd.RestartIfExists() && fExists) || (cmd.RestartIfNotExists() && !fExists))
            {
                if (i > 0)
                    i = -1;
            }
            continue;
        }
        else if(cs == TCommand::EIf || cs == TCommand::EIfn)
        {
            TBool fExists = IsPathExists(cmd.GetSrc());

            //continue next cmd
            if ((cs == TCommand::EIf && fExists) || (cs == TCommand::EIfn && !fExists))
            {
                fLastCondition = ETrue;
                continue;
            }

            fLastCondition = EFalse;
            SkipCmd(aCmdSet, i);    //otherwise, skip the cmds of if(n) statement
            continue;
        }
        else if(cs == TCommand::EElseIf || cs == TCommand::EElseIfn)
        {
            if (!fLastCondition)
            {
                TBool fExists = IsPathExists(cmd.GetSrc());
                if ((cs == TCommand::EElseIf && fExists) || (cs == TCommand::EElseIfn && !fExists))
                {
                    fLastCondition = ETrue;
                    continue;
                }
                else
                    fLastCondition = EFalse;
            }
            
            SkipCmd(aCmdSet, i);
            continue;
        }
        else if(cs == TCommand::EElse)
        {
            if (!fLastCondition)
                continue;

            SkipCmd(aCmdSet, i);
            continue;
        }
        else if(cs == TCommand::EEndIf)
        {
            continue;
        }

        DoCommand(cmd);
    }
}
//=================================================================================
void SkipCmd(const CArrayFixFlat<TCommand> &aCmdSet, TInt &i)
{
    for(++i; i<aCmdSet.Count(); i++)
    {
        TCommand iCmd = aCmdSet.operator [](i);
        if (iCmd.GetCommand() == TCommand::EElse ||
            iCmd.GetCommand() == TCommand::EElseIf ||
            iCmd.GetCommand() == TCommand::EElseIfn ||
            iCmd.GetCommand() == TCommand::EEndIf)
        {
            i--;    //back to current found cmd
            break;
        }  
    }
}
//=================================================================================
void MakeAtt(TUint &aSet, TUint &aClear, const Parameter &aParam)
{
    if (aParam.s == add_to)
        aSet |= KEntryAttSystem;
    if (aParam.h == add_to)
        aSet |= KEntryAttHidden;
    if (aParam.r == add_to)
        aSet |= KEntryAttReadOnly;

    if (aParam.s == rm_it)
        aClear |= KEntryAttSystem;
    if (aParam.h == rm_it)
        aClear |= KEntryAttHidden;
    if (aParam.r == rm_it)
        aClear |= KEntryAttReadOnly;
}
//=================================================================================
TInt RunApp(const TDesC &aName)
{
    RProcess process;

    TInt ret = process.Create(aName, TPtr(NULL, 0));
    process.Resume();

    return ret;
}
//=================================================================================
TInt KillApp(TInt aUid)
{
    TInt ret = KErrNone;

    TFullName pName;
    TFindProcess finder(_L("*"));

    TUid tUid = {aUid};

    while(ETrue)
    {
        finder.Next(pName);

        if (pName != KNullDesC)
        {
            RProcess process;

            ret = process.Open(pName);
            if (ret != KErrNone)
                return ret;

            if (tUid == process.Type()[2])
            {
                process.Kill(0);
                process.Close();
                break;
            }

            process.Close();
        }
        else
            break;
    }

    return ret;
}
//=================================================================================
TInt KillApp(const TDesC &aName)
{
    TInt ret = KErrNone;

    TFullName pName;
    pName.Copy(aName);
    pName.Append(_L("*"));

    TFindProcess finder(pName);

    while(ETrue)
    {
        finder.Next(pName);

        if (pName != KNullDesC)
        {
            RProcess process;

            ret = process.Open(pName);
            if (ret != KErrNone)
                return ret;

            process.Kill(0);
            process.Close();

            break;
        }
        else
            break;
    }

    return ret;
}
//=================================================================================
TInt DoKill(const TDesC &aStr)
{
    if (aStr.Length() >= 8) //0x12345678
    {
        if (aStr[0] == '0' && (aStr[1] == 'x' || aStr[1] == 'X'))
        {
            TBuf<16> iUid;
            iUid.Copy(aStr);
            iUid.Delete(0, 2);
            
            return KillApp(HexStr2Int32(iUid));
        }
    }
    
    return KillApp(aStr);
}
//=================================================================================
void Sleep(TInt aMs)
{
    User::After(aMs * 1000);
}
//=================================================================================
TInt GetHex(char hex)
{
    if (hex >= '0' && hex <= '9')
        return hex - '0';

    if (hex >= 'a' && hex <= 'f')
        return hex - 'a' + 10;

    if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 10;
    
    return 0;
}
//=================================================================================
TInt32 HexStr2Int32(const TDesC & aHexStr)
{
    TInt len = aHexStr.Length();
    //防止溢出
    if(len > 8) return 0;
    
    TInt32 res = 0; 
    TInt32 tmp = 0;
    const TUint16 * hexString = aHexStr.Ptr();
    
    for (TInt i = 0; i < len; i++)
    {
        tmp = GetHex(hexString[i]);
        
        tmp <<= ((len-i-1)<<2);    
        res |= tmp;   
    }

    return res;
}
//=================================================================================
TInt32 DecStr2Int32(const TDesC &aDecStr)
{
    TInt len = aDecStr.Length();
    TInt i = 0;
    TReal res = 0;
    
    for (TInt j=len-1; j>=0; j--)
    {
        TReal pow = 0;
        Math::Pow10(pow, i++);
        
        res = (aDecStr[j] - '0') * pow + res;
    }
    
    return (TInt32)res;
}
//=================================================================================
///////////////////////////////////////////////////////////////////////////////////////////////
//END OF FILE
///////////////////////////////////////////////////////////////////////////////////////////////
