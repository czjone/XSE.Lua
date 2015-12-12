@echo off
set luaver = 5.1.5
set currentpath = %~dp0
@echo "------------------------start build lua extetion lib---------------------"
cd %~dp0
dir
cl a.c
@echo "------------------------build lua extetion lib complate------------------"
pause