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
 * TFileMonitor.h
 *
 *  Created on: 2011-6-3
 *      Author: Tim
 */

#ifndef __TFILEMONITOR_H__
#define __TFILEMONITOR_H__

#include <f32file.h>

class TFileMonitor : public MFileManObserver
{
public:
    enum TMonitorMode  {   ENone = -1, ECopy, EDelete  };
    TFileMonitor(CFileMan& aFileMan);

    void SetMode(TMonitorMode m)    {   mode = m;   }
    void SetAtt(TUint aSetAttMask, TUint aClearAttMask)
    {
        iSetAttMask = aSetAttMask;
        iClearAttMask = aClearAttMask;
    }
public:
    TControl NotifyFileManStarted();
    TControl NotifyFileManOperation();
    TControl NotifyFileManEnded();
    
protected:
    CFileMan& iFileMan;
    TUint iSetAttMask;
    TUint iClearAttMask;
    
    TMonitorMode mode;
};

#endif /* __TFILEMONITOR_H__ */
