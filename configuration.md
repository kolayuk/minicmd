# Configuration for MiniCMD (for version v2.1 or higher) #

**template**

```

; All the option is in the MiniCMD section
; A line first character is semicolon means this line only for comments

[MiniCMD]

; Enable MiniCMD
Enable=1

; Only search the command file in the E drive and C drive
; First E and then Z
; You can specific other drives like D, Z
Drive=EC

; Default command file path
CMDFile=Data\MiniCMD.txt

; OPTIONAL
; If you want to specific the command file name in drive C,
; then you can add a line CMDFileC
; other drives are D, E, Y and Z
CMDFileC=C:\MyCMD\CMD.txt

; Log level for minicmd
; 0 - disabled
; 1 - only for failed cmd
; 2 - all
LogLevel=2

; Where the log file you want to save in
LogFile=D:\MiniCMD.log

```


# Explanation #

This file default should place in _DRIVE_:\System\Apps\MiniCMD\_MiniCMD.ini_

_DRIVE_ can be **C**, **D**, **E** and **Z**

By default, MiniCMD search start from **D**, and then **C**, **E**, the last is **Z**

Most of the time _MiniCMD.ini_ isn't the for you, if you change MiniCMD.exe to your own, e.g. MyCMD.exe.

Then you should change _MiniCMD.ini_ to _MyCMD.ini_


**NOTE**

File encoding only supports UCS-2 Little Endian