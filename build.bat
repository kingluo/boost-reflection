@echo off
call "%VS90COMNTOOLS%vsvars32.bat"
mkdir build
cd build
cmake -G "Visual Studio 9 2008" ../
IF NOT ERRORLEVEL 0 goto BUILD_ERROR
devenv boost-reflection.sln /Build "Release|Win32"
GOTO END

:BUILD_ERROR
pause
exit /b 1

:END
pause
