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
#include <apgcli.h>
#include <apacmdln.h>
#include <BAUTILS.H>
#include <e32math.h>

#include <e32cons.h>

#include "MiniCMD.h"
//=================================================================================
_LIT(KDefBatFileC, "C:\\MiniCmd.bat");
_LIT(KDefBatFileE, "E:\\MiniCmd.bat");
_LIT(KDefBatFileD, "D:\\MiniCmd.bat");
_LIT(KDefBatFileZ, "Z:\\MiniCmd.bat");

_LIT(KDisableFile, "MiniCmd.disable");

RFs iFs;

CFileMan *fileMan = NULL;
//for debug only
//print information is more comfortable
//than write log file 
//LOCAL_D CConsoleBase* console = NULL;
//=================================================================================
LOCAL_C void MainL()
{ 
 /*
    TRAPD(createError, console = Console::NewL(_L("Console"), TSize(KConsFullScreen, KConsFullScreen)));
    if (createError){
        return;
    }
*/
    User::LeaveIfError(iFs.Connect());
    fileMan = CFileMan::NewL(iFs);
    if (!fileMan)
        return ;

    if (!IsCmdDisabled())
        ParseBatFileL();
}
//=================================================================================
GLDEF_C TInt E32Main()
{     
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
 
    TRAPD(iErr, MainL());

    delete fileMan;
    iFs.Close();
    delete cleanup;

//    delete console;
    
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
    aLine.Trim();
  
    if (0 == aLine.Length())    // Empty line
        return ;

    if ('#' == aLine[0] || ';' == aLine[0]) // Comment line
        return ;
    
    TBuf<3> cmd;

    cmd.Copy(aLine.Left(3));
    cmd.Trim();
    cmd.LowerCase();    //case insensitive
    
    //att cp mv md rn rm
    if (cmd == _L("att"))
    {
        aLine.Delete(0, 3);
        aLine.TrimLeft();
        
        TFileName iSrc;
        Param iParam;
        
        ParseCMD(aLine, iSrc, NULL, &iParam);
        
        iSrc.Trim();
        if (iParam.NeedToSetAtt() && iSrc.Length() > 0)
            SetAtt(iSrc, iParam);
    }
    else if(cmd == _L("cp") || cmd == _L("mv"))
    {
        aLine.Delete(0, 2);
        aLine.TrimLeft();
        
        //optional parameters
        Param iParam;
        TFileName iSrc, iDest;
        
        ParseCMD(aLine, iSrc, &iDest, &iParam);
        iSrc.Trim();
        iDest.Trim();
        if (iSrc.Length() <= 0 || iDest.Length() <= 0)
            return ;
        
        if (cmd == _L("cp"))
            Copy(iSrc, iDest, iParam);
        else
            Move(iSrc, iDest, iParam);
    }
    else if(cmd == _L("md"))
    {
        aLine.Delete(0, 2);
        aLine.TrimLeft();
        TFileName iSrc;
        if (!FindPath(aLine, iSrc))
            return ;
        
        iSrc.Trim();
        
        if (iSrc.Length() > 0)
        {
            //Seems RFs::MkDirAll need a back slash
            if (iSrc[iSrc.Length() - 1] != '\\')
                iSrc.Append('\\');
            
            MkDir(iSrc);
        }
    }
    else if(cmd == _L("rn"))
    {
        aLine.Delete(0, 2);
        aLine.TrimLeft();

        TFileName iSrc, iDest;
        
        ParseCMD(aLine, iSrc, &iDest);
        iSrc.Trim();
        iDest.Trim();
        if (iSrc.Length() > 0 && iDest.Length() > 0)
            Rename(iSrc, iDest);
    }
    else if(cmd == _L("rm"))
    {
        aLine.Delete(0, 2);
        aLine.TrimLeft();

        Param iParam;
        TFileName iSrc;
        
        ParseCMD(aLine, iSrc, NULL, &iParam);
        iSrc.Trim();
        if (iSrc.Length() > 0)
            Delete(iSrc, iParam.is);
    }
    else if(cmd == _L("run"))
    {
        aLine.Delete(0, 3);
        aLine.TrimLeft();

        TBuf<32> uid;
        if (!FindPath(aLine, uid))
            return ;

        uid.Trim();

        if (uid.Length() > 2)   //其实UID一般是8位的十六进制。。。
        {      
            TUid tuid;
            //TLex lex(uid);
            
            if (uid[0] == '0' && (uid[1] == 'x' || uid[1] == 'X'))
            {
                uid.Delete(0, 2);   //remove 0x
                //lex.Val(tuid.iUid, EHex);
                tuid.iUid = HexStr2Int32(uid);
            }
            else
            {
                //lex.Val(tuid.iUid, EDecimal);
                tuid.iUid = DecStr2Int32(uid);
            }
/*            
            console->Write(uid);
            console->Write(_L("\n"));
            console->Printf(_L("HEX: %d\n"), tuid.iUid);
            console->Getch();*/
            LaunchAppL(tuid);
        }
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
TInt GetParams(TDes &aLine, Param &aParam)
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

    TBool iRet;
    //这SB函数把带有通配符的或者不存在的文件(夹)全认作文件夹!!!
    BaflUtils::IsFolder(iFs, aPath, iRet);
    return iRet;
}
//=================================================================================
TInt SetAtt(const TDesC &aPath, const Param &aParam)
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
TInt Copy(const TDesC &aSrc, const TDesC &aDest, const Param &aParam)
{
    TInt ret = KErrNone;
    if (IsDir(aSrc))
        ret = fileMan->Copy(aSrc, aDest, aParam.ow ? CFileMan::EOverWrite | CFileMan::ERecurse : CFileMan::ERecurse);
    else
        ret = fileMan->Copy(aSrc, aDest, aParam.is ? (
            aParam.ow ? CFileMan::EOverWrite | CFileMan::ERecurse : CFileMan::ERecurse)
            : ( aParam.ow ? CFileMan::EOverWrite : 0));
  
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
    Param iParam;
    iParam.h = rm_it;
    iParam.s = rm_it;
    iParam.is = ETrue;
    SetAtt(aSrc, iParam);   //去除只读属性，确保能删除
    
    if (IsDir(aSrc))
    {
        TFileName buf;
        buf.Copy(aSrc);
        
        //如果最后的字符不是'\'
        if (aSrc[aSrc.Length()-1] != '\\'){
            buf.Append('\\');
        }
  
        return fileMan->RmDir(buf);
    }
    else
    {
        return fileMan->Delete(aSrc, aIncludeSubdir ? CFileMan::ERecurse : 0);
    }
}
//=================================================================================
TInt Move(const TDesC &aSrc, const TDesC &aDest, const Param &aParam)
{
    TInt ret = KErrNone;
    if (IsDir(aSrc))
    {
        ret = fileMan->Move(aSrc, aDest, aParam.ow ? CFileMan::ERecurse | CFileMan::EOverWrite : CFileMan::ERecurse);
    }
    else
    {
        if (aParam.ow)
            ret = fileMan->Move(aSrc, aDest, aParam.is ? CFileMan::ERecurse | CFileMan::EOverWrite : CFileMan::EOverWrite);
        else
            ret = fileMan->Move(aSrc, aDest, aParam.is ? CFileMan::ERecurse : 0);
    }
    
    return ret;
}
//=================================================================================
void LaunchAppL(const TUid aAppUid)
{
    RApaLsSession apaLsSession;
    User::LeaveIfError(apaLsSession.Connect());
    CleanupClosePushL(apaLsSession);

    TApaAppInfo appInfo;
    TInt retVal = apaLsSession.GetAppInfo(appInfo, aAppUid);

    if(retVal == KErrNone)
    {
        CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
        cmdLine->SetExecutableNameL(appInfo.iFullName);
        cmdLine->SetCommandL(EApaCommandRun);
        User::LeaveIfError( apaLsSession.StartApp(*cmdLine) );

        CleanupStack::PopAndDestroy(cmdLine);
        
//        console->Write(appInfo.iFullName);
//        console->Getch();
    }
/*    else
    {
        console->Write(_L("Not found App\n"));
        console->Getch();
    }
*/
    CleanupStack::PopAndDestroy(&apaLsSession);
}
//=================================================================================
void ParseCMD(TDes &aCMD, TDes &aSrc, TDes *aDest/* = NULL*/, Param *aParam/* = NULL*/)
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

        //Important to +1 when the first path enclosed by quote
        aCMD.Delete(0, aSrc.Length() + 1);
        aCMD.TrimLeft();

        FindPath(aCMD, *aDest);
    }
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
    TInt32 res = 0;
    
    for (TInt j=len-1; j>=0; j--)
    {
        TReal pow = 0;
        Math::Pow10(pow, i++);
        
        res = (aDecStr[j] - '0') * pow + res;
    }
    
    return res;
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
