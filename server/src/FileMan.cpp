//=================================================================================
#include "FileMan.h"
#include "TFileMonitor.h"
//=================================================================================
RFs iFs;
CFileMan *fileMan = NULL;
TFileMonitor *fileMonitor = NULL;
//=================================================================================
void OpenFileSession()
{
    User::LeaveIfError(iFs.Connect());
    fileMan = CFileMan::NewL(iFs);
    
    fileMonitor = new TFileMonitor(*fileMan);
    fileMan->SetObserver(fileMonitor);
}
//=================================================================================
void CloseFileSession()
{
    delete fileMonitor;
    delete fileMan;
    
    iFs.Close();
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

    TEntry entry;
    if (iFs.Entry(aPath, entry) != KErrNone)
        return entry.IsDir();

    return EFalse;
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
TInt MkDir(const TDesC &aPath)
{
    return iFs.MkDirAll(aPath);
}
//=================================================================================
TInt SetAtt(const TDesC &aPath, const Parameter &aParam)
{
    TUint iSetAttMask = KEntryAttNormal;
    TUint iClearAttMask = KEntryAttNormal;

    //if no attributes needs to change
    //just return back
    if (!aParam.NeedToSetAtt())
        return KErrGeneral;
    
    MakeAtt(iSetAttMask, iClearAttMask, aParam);

    TInt ret = KErrNone;
    //use synchronous
    if (aParam.is)
    {
        ret = fileMan->Attribs(aPath, iSetAttMask, iClearAttMask, 0, CFileMan::ERecurse);
        
        if (aPath[aPath.Length()-1] == '\\')    //CFileMan::Attribs make no sense to this case
            iFs.SetAtt(aPath, iSetAttMask, iClearAttMask);
    }
    else
    {
        if (aPath[aPath.Length()-1] == '\\')    //set folder only
            ret = iFs.SetAtt(aPath, iSetAttMask, iClearAttMask);
        else
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
    if (aSrc.Length() == 0 || aDest.Length() == 0)
        return KErrGeneral;

    if (aSrc[aSrc.Length() - 1] == '\\')    //rename a folder
        return iFs.Rename(aSrc, aDest);

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
            ret = fileMan->Move(aSrc, aDest, aParam.is ?- CFileMan::ERecurse | CFileMan::EOverWrite : CFileMan::EOverWrite);
        else
            ret = fileMan->Move(aSrc, aDest, aParam.is ? CFileMan::ERecurse : 0);
    }

    fileMonitor->SetMode(TFileMonitor::ENone);
    
    return ret;
}
//=================================================================================
TInt CreateFile(const TDesC &aFileName)
{
    RFile file;
    TInt ret = file.Create(iFs, aFileName, EFileWrite);
    file.Close();
    return ret;
}
//=================================================================================

//TInt CopyF(const TDesC &aSrc, const TDesC &aDest, TBool aOverwiritten/* = EFalse*/)
/*{
    TEntry entry;
    if (!aOverwiritten && iFs.Entry(aDest, entry) == KErrNone)  //file exists
        return KErrAlreadyExists;
    
    RFile file;
    TFileName fileName;
    fileName.Copy(aDest);
    TParse parse;
    iFs.Parse(aSrc, parse);
    
    if (fileName[fileName.Length()-1] == '\\')
        fileName.Append(parse.Name());
    
    MkDir(aDest);

    TInt ret = file.Replace(iFs, fileName, EFileWrite);
    if (ret != KErrNone)
        return ret;
    
    TInt pos = 0;
    HBufC8 *fileBuf = HBufC8::NewLC(256);
    TPtr8 ptr = fileBuf->Des();
    
    while(ETrue)
    {
        ret = iFs.ReadFileSection(aSrc, pos, ptr, 256);

        if (ret == KErrNone)
        {
            ret = file.Write(ptr);
            if (ret != KErrNone)
                break;
            ret = file.Flush();
            if (ret != KErrNone)
                break;
        } 
        else
            break;
        
        pos += 256;
    }
    
    CleanupStack::PopAndDestroy(fileBuf);
    file.Close();
    
    if (ret == KErrEof)
        ret = KErrNone;
    
    return ret;
}*/
//=================================================================================
///////////////////////////////////////////////////////////////////////////////////
//END OF FILE
///////////////////////////////////////////////////////////////////////////////////
