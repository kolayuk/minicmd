/*
 ============================================================================
 Name		    : MiniCMD.h
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
#   命令集
#
#   att
#   设置文件（夹）属性
#   参数 +s +h +r分别为加系统、隐藏、只读属性，
#   相应的'+'改为'-'就是去除属性
#   /s参数指定包括子目录
#
#   例：att -r e:\test\*，去除e:\test\目录下所有文件（夹）的只读属性（不包括子目录）
#
#   cp
#   复制文件（夹），两个参数，第一个为源文件（夹），第二个为目的
#   当复制的是目录时，同时复制全部的文件和子目录
#   可用参数：/s，/o（或者写成-o也可以），/s功能同att
#   /o(-o)表示覆盖已存在文件，不加则默认不覆盖
#   /s 应仅当使用于复制文件时，目录的复制本身已包括子目录
#   例：cp e:\test\test.txt e:\test.doc
#
#   mv
#   移动文件（夹），参数、用法同cp
#
#   md
#   创建目录，只有一个参数，为新建目录路径
#   例：md e:\test
#
#   rn
#   重命名文件（夹），两个参数，第一个为源，第二个为目的
#
#   rm
#   删除文件（夹），只有一个目的参数
#   可选用/s参数指定包括子目录里的文件（仅用于删除文件时）
#
#   run
#   运行程序，参数为程序的UID（如果是十六进制形式，必须以0x或0X开头，其它形式认作10进制）
#   例：run 0X2001F848
#
#注：
#   可使用的通配符
#   * 代表一串字符
#   ? 代表一个字符
#当指定路径存在空格时用双引号包含路径，比如e:\my file\, 应写成"e:\my file\"
#否则得不到期待结果
#各个参数不区分大小写，但'-'、'\'和各字母之间不能有空格，字母后也必需留空！
#本文件编码只能为UCS-2 Little Endian，即UTF-16 LE（通常说的Unicode）
#注释行用#或者英文分号';'
#将些文件放置于任一盘的根目录，比如C:\MiniCmd.bat
 ============================================================================
 */
#ifndef __MINI_CMD_H__
#define __MINI_CMD_H__
//=================================================================================
#include <e32base.h>
//=================================================================================
enum AttStatus
{
    not_set = 0,    //不设置
    add_to,     //+
    rm_it       //-
};

struct Param
{
    AttStatus s;    //sys attribute
    AttStatus h;    //hidden
    AttStatus r;    //read only
    
    TBool is;       //include subfolder?
    TBool ow;       //overwrite exists file?
    
    Param()
    {
        s = h = r = not_set;
        is = ow = EFalse;
    }
    
    TBool NeedToSetAtt() const{
        return s + h + r > 0;
    }
    
    Param & operator = (const Param &other)
    {
        this->s = other.s;
        this->h = other.h;
        this->r = other.r;
        this->is = other.is;
        this->ow = other.ow;
        
        return *this;
    }
};

//=================================================================================
TInt    ParseBatFileL();
void    ParseLineL(TDes &aLine);

//取得命令行中的一个路径
TBool   FindPath(TDes &aLine, TDes &aPath);

// 返回参数所占长度
TInt    GetParams(TDes &aLine, Param &aParam);

TInt    MkDir(const TDesC &aPath);
TBool   IsDir(const TDesC &aPath);
TInt    SetAtt(const TDesC &aPath, const Param &aParam);
TInt    Copy(const TDesC &aSrc, const TDesC &aDest, const Param &aParam);
TInt    Rename(const TDesC &aSrc, const TDesC &aDest);
TInt    Delete(const TDesC &aSrc, TBool aIncludeSubdir);
TInt    Move(const TDesC &aSrc, const TDesC &aDest, const Param &aParam);
void    LaunchAppL(const TUid aAppUid);
void    ParseCMD(TDes &aCMD, TDes &aSrc, TDes *aDest = NULL, Param *aParam = NULL);
TInt32  HexStr2Int32(const TDesC &aHexStr);
TInt32  DecStr2Int32(const TDesC &aDecStr);

TBool   IsCmdDisabled();
//=================================================================================
#endif  // __MINI_CMD_H__
//=================================================================================
