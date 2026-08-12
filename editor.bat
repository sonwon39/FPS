@echo off

call %~dp0\vars.bat

start "" "%UE5EDITOR_EXE%" "%UPROJECT_PATH%" %*