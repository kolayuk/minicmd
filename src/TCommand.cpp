/*
 * TCommand.cpp
 *
 *  Created on: 2011-5-27
 *      Author: Tim
 */

#include "TCommand.h"

void TCommand::SetCommand(ECommandSet aCmd, const Parameter *aParam, const TDesC *aSrc, const TDesC *aDst/* = NULL*/)
{
    iCmd = aCmd;

    if (aParam){
        iParam = *aParam;
    }

    if (aSrc && aSrc->Length() > 0){
        iSrc.Copy(*aSrc);
    }

    if (aDst && aDst->Length() > 0){
        iDst.Copy(*aDst);
    }
}
