/*
 * TCommand.h
 *
 *  Created on: 2011-5-27
 *      Author: Tim
 */

#ifndef __TCOMMAND_H__
#define __TCOMMAND_H__

#include <e32base.h>

#include "Parameter.h"

class TCommand
{
public:
    enum ECommandSet
    {
        ENone = -1, //no command
        EAtt = 0,   //attribute
        ECp,        //copy
        EMv,        //move
        EMd,        //make dir
        ERn,        //rename
        ERm,        //remove
        EDel,       //delete (file only)
        EAbes,      //abort if exists
        EAbnes,     //abort if not exists
        ESkes,      //skip if exists
        ESknes,     //skip if not exists
        EBkes,      //back to last cmd if exists
        EBknes,     //back to last cmd if not exists
        ERses,      //restart if exists
        ERsnes      //restart if not exists
    };
    
    TCommand() : iCmd(ENone){}
    TCommand(ECommandSet aCmd, const Parameter *aParam, const TDesC *aSrc, const TDesC *aDst = NULL){
        SetCommand(aCmd, aParam, aSrc, aDst);
    }
    virtual ~TCommand(){}
public:
    void SetCommand(ECommandSet aCmd, const Parameter *aParam, const TDesC *aSrc, const TDesC *aDst = NULL);
    
    ECommandSet GetCommand()        const   {    return iCmd;    }
    const Parameter & GetParam()    const   {    return iParam;  }
    
    const TDesC & GetSrc() const        {    return iSrc;    }
    const TDesC & GetDst() const        {    return iDst;    }
    
    TBool AbortIfExists()       const   {   return iCmd == EAbes;   }
    TBool AbortIfNotExists()    const   {   return iCmd == EAbnes;  }
    TBool SkipIfExists()        const   {   return iCmd == ESkes;   }
    TBool SkipIfNotExists()     const   {   return iCmd == ESknes;  }
    TBool BackIfExists()        const   {   return iCmd == EBkes;   }
    TBool BackIfNotExists()     const   {   return iCmd == EBknes;  }
    TBool RestartIfExists()     const   {   return iCmd == ERses;   }
    TBool RestartIfNotExists()  const   {   return iCmd == ERsnes;  }

protected:
    
    ECommandSet iCmd;   //what command
    Parameter   iParam; //
    TFileName   iSrc;   //source path
    TFileName   iDst;   //destination path
};

#endif /* __TCOMMAND_H__ */
