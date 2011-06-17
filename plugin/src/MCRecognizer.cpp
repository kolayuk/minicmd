
#include <ImplementationProxy.h>

#include "MCRecognizer.h"

const TUid KUidMCS  = { 0xE132DCB5 };   //DLL UID
//const TUid KUidMC   = { 0xE132DCB3 };   //UID of MiniCMD

_LIT(KMiniCmdFile, "\\sys\\bin\\MiniCMD.exe");

CApaDataRecognizerType* CMCRecognizer::NewL()
{
    CMCRecognizer* mcr = new CMCRecognizer();
    CMCRecognizer::RunMiniCMD();
    return mcr;
}

CMCRecognizer::CMCRecognizer()
    :CApaDataRecognizerType(KUidMCS, CApaDataRecognizerType::ENormal)
{
    iCountDataTypes = 1;
}

TDataType CMCRecognizer::SupportedDataTypeL(TInt /*aIndex*/) const
{
    return TDataType();
}

void CMCRecognizer::RunMiniCMD()
{
    RProcess process;
    if (process.Create(KMiniCmdFile, KNullDesC) == KErrNone)
    {
        process.Resume();
        process.Close();
    }
}

const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(0xE132DCB5, CMCRecognizer::NewL)
};

EXPORT_C const TImplementationProxy * ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(ImplementationTable) / sizeof(* ImplementationTable);

    return ImplementationTable;
}
