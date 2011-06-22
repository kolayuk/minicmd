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
#include <ImplementationProxy.h>

#include "MCRecognizer.h"

const TUid KUidMCS  = { 0xE132DCB5 };   //DLL UID

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
