@echo off

set ROOTDIR=%~dp0
set ROOTDIR=%ROOTDIR:~0,-1%

set PROJECT=FPS
set PROJECT_DIR=%ROOTDIR%
set UPROJECT_PATH=%PROJECT_DIR%\%PROJECT%.uproject
set GAME_EXE=%PROJECT_DIR%\Binaries\Win64\%PROJECT%.exe

set UE5_DIR=D:\UE_5.8
set BUILD_BAT=%UE5_DIR%\Engine\Build\BatchFiles\Build.bat
set RUNUAT_BAT=%UE5_DIR%\Engine\Build\BatchFiles\RunUAT.bat
set UE5EDITOR_EXE=%UE5_DIR%\Engine\Binaries\Win64\UnrealEditor.exe
set UE5EDITOR_CMD_EXE=%UE5_DIR%\Engine\Binaries\Win64\UnrealEditor-cmd.exe