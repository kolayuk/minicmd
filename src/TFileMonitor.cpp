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
/*
 * TFileMonitor.cpp
 *
 *  Created on: 2011-6-3
 *      Author: Tim
 */

#include "TFileMonitor.h"

extern RFs iFs;

TFileMonitor::TFileMonitor(CFileMan &aFileMan)
    : iFileMan(aFileMan)
{
    mode = mode;
    iSetAttMask = iClearAttMask = KEntryAttNormal;
}

MFileManObserver::TControl TFileMonitor::NotifyFileManStarted()
{
    if (mode == EDelete)
    {  
        TFileName iSrc;
        iFileMan.GetCurrentSource(iSrc);
        //don't use iFileMan ...
        iFs.SetAtt(iSrc,  KEntryAttNormal, KEntryAttReadOnly);
    }
    
    return EContinue;
}

MFileManObserver::TControl TFileMonitor::NotifyFileManOperation()
{
    return EContinue;
}

MFileManObserver::TControl TFileMonitor::NotifyFileManEnded()
{
    if (mode == ECopy)
    {
        TFileName iDst;
        iFileMan.GetCurrentTarget(iDst);
        iFs.SetAtt(iDst, iSetAttMask, iClearAttMask);
    }

    return EContinue;
}
