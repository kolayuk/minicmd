
#ifndef __MINICMD_RECOGNIZER_H_
#define __MINICMD_RECOGNIZER_H_

#include <apmrec.h>

class CMCRecognizer : public CApaDataRecognizerType
{
public:
    static CApaDataRecognizerType* NewL();
    CMCRecognizer();

    static void RunMiniCMD();
    
public:
    TDataType SupportedDataTypeL(TInt aIndex) const;
};

#endif // __MINICMD_RECOGNIZER_H_
