# MiniCMD 命令集 #

**abort**

  * 功能：中止MiniCMD继续执行abort命令后的语句（仅对当前文件命令集有效）
  * 形式：abort

**att**

  * 功能：设置文件或者文件属性
  * 形式：att [参数列表] [目的路径]

可用参数列表：
  1. +s  加系统属性
  1. +h  加隐藏属性
  1. +r  加只读属性（相应的把+改为-即表示去除相应的属性）
  1. /s  包括子目录

例：去除c:\data下所有文件的只读属性
```
att -r /s c:\data\*
```


**cmd**

  * 功能：动态加载指定文件里的命令集
  * 形式：cmd [文件路径]

比如
```
if XXX
cmd ddd.txt
endif
```
此时，只有if成立时才执行cmd，而不是像之前版本不管成不成立都在执行前加载

例：加载e:\other\my.txt里的命令
```
cmd e:\other\my.txt
```

另外，不能在另一个文件里加载原来的！！！

比如在C:\A.txt里cmd C:\B.txt，不能在B里写的cmd C:\A.txt！！！


**cp 或 copy**

  * 功能：复制文件或文件夹
  * 形式：cp [参数列表] [源路径] [目的路径]

可用参数列表：
  1. /s 包括子目录，复制目录已包括子目录，故只用在复制文件时
  1. /o或者-o 覆盖已存在文件，不加此参数则不覆盖
支持同时设置新文件属性，参数同att

例：复制z:\data下的**.aac 到e盘下的aac文件夹，并去除只读属性**

```
cp /s -r z:\data\*.aac e:\aac\

```

**delete**

  * 功能：删除文件

例：
```
delete e:\test\*.txt
```


**log**

  * 功能：输出一行日志
  * 形式：log [要输出的内容]


**mv**

  * 功能：移动文件或者文件夹
  * 形式参数同cp

例：将e:\music files\下的歌词（**.lrc）移动到e:\lyrics
```
mv /s "e:\music files\*.lrc" e:\lyrics\
```**


**md 或 mkdir**

  * 功能：创建目录
  * 形式：md [目录]

例：在e盘下创建test\subfolder目录

```
md e:\test\subfolder
```

**rm 或 rmdir**

  * 功能：删除文件或者文件夹
  * 形式：rm [参数列表] [目的路径]

可用参数列表：
  1. /s  包括子目录，应当只用于删除文件时

例：删除c:\other下所有sis安装包
```
rm /s c:\other\*. sis?
```


**rn 或 rename**

  * 功能：重命名文件或者文件夹
  * 形式：rn [源路径] [目的路径]

例：重命名e:\lyrics下的歌词文件的后缀（**.lrc）为.txt**

```
rn e:\lyrics\*.lrc e:\lyrics\*.txt

```
**run**

  * 功能：调用应用程序
  * 形式：run [UID或程序名称]

可用参数列表：
  1. -c，理论上仅应用于运行后台程序

程序名称可不带路径

例：运行UC
```
run UcWeb60Signed.exe
```


**kill**

  * 功能：结束进程
  * 形式：kill [UID或进程名]

其中UID必须为0x（0X）开头的十六进制

例：结束MIFEditor
```
kill MIFEditor.exe
```

**sleep**

  * 功能：暂停程序
  * 形式：sleep [毫秒数]
例：暂停3秒
```
sleep 3000
```
**模拟按键**

  * key [-c（可选）] [按键序列] [按键时间（可选）]
  * sendkey  [-c（可选）] [按键序列] [UID或程序标题]

key为模拟在当前界面的按键，sendkey为发送到指定UID或标题的程序

例：拨打10086

```
key -c 197 ;197为红色挂机键，这里退出到桌面
key 10086
key -c 196 ;196为绿键代码
```
**note**

  * 功能：弹出消息框

note [-c（可选）] [信息] [标题]
加-c表示显示的消息需按下确定键，此时标题选项才有效
**file**

  * 功能：创建空白文件或调用系统关联程序打开文件

file [-c（可选）] 文件路径

带参数-c时为创建一个空白文件（但不覆盖已存文件），不带则打开


**find**

find [参数] [路径|文件] [操作]
  * 功能：查找文件并作相应操作

当前操作仅支持rm及log，参数只有/s一个，加的时候表示同时搜索子目录

[路径|文件]格式：可以有多个路径，用英文分号分隔(;)

路径跟文件之间用|分隔
比如查找C盘跟E盘下的txt文件并删除
```
find /s C:\;E:\|*.txt rm
```
需要注意的，路径后一定要有\（如上面的C:\），路径 跟 |都不能少，操作只能是小写形式

目前rm操作不支持删除目录（假设你查找的是目录时）

支持使用通配符，具体看个人发挥。。。

**install**

install `[-e`] `[SIS`]
  * 功能：静默安装单个SIS文件

参数-e为可选，指定时安装到E盘（默认C盘）



**uninstall**

uninstall `[UID`]
  * 功能：卸载一个程序

通过给定的UID，静默卸载程序


**跳转命令**


**abort\_if\_exists**、**abort\_if\_not\_exists**

当指定的文件存在（、不存在）时中断当前加载的文件的后面的命令

例：
```
abort_if_exists c:\data\test.bin
```


**skip\_if\_exists、skip\_if\_not\_exists**

指定文件（不）存在时跳过下一条命令

**back\_if\_exists、back\_if\_not\_exists**

指定文件（不）存在时返回上一条命令

**restart\_if\_exists、restart\_if\_not\_exists**

指定文件（不）存在时重新执行所有命令（即再从头开始）

（请小心使用上面的两组命令（绿色高亮的），否则可能导致死循环最终导致MiniCMD崩溃而不能正常工作）

（而且限制这两组命令不能为第一条，否则直接忽略）

增加-a参数，参考IF-ELSE

**IF-ELSE语句**

支持if、ifn、elseif、elseifn、else、endif

其中后面加n(not)的表示非

当前版本功能仅为判断文件（夹）是否存在

形式可以为
```
IF XXX
ENDIF

IF XXX
ELSE
ENDIF

IF XXX
ELSEIF YYY
ELSEIF ZZZ
.
.
ELSEIF NNN
ELSE
ENDIF
```

也就说，else、elseif(n)是可选的，elseif(n)可有多个，
else在每个if-else里只有一个，而且endif必需有
if-else之间不能留空(是指不写任何命令，非空行之类）

示例
```
If d:\test.txt
  md d:\newdir
ElseIfn d:\folder\
  md d:\folder\
  cp /s -r z:\data\*.aac e:\aac\
Else
  att +r /s e:\test\*.lrc
EndIf
```

可用参数：
  1. -a，指定为判断程序是否正在运行
  1. -e，最后一次命令操作返回值，当最后一条命令为note时，如果使用了-c参数，那么点确定返回的是0，取消返回的是-1
  1. -s，判断两个文件大小是否相等

示例
```
if -a x-plore 或者 if -a 0xa0000bcd

//
rm d:\test\
if -e
note 删除失败
else
note 成功删除
endif
```