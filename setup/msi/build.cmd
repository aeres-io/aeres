@echo off

if "%~1"=="" goto help

SET DEVROOT=..\..

SET PKGNAME=aeres

SET TARGET_CPU=%1

SET /P AERES_VERSION=< "%DEVROOT%\VERSION"

IF NOT EXIST "%DEVROOT%\out\pkg" md "%DEVROOT%\out\pkg"

candle %PKGNAME%.wxs
light -ext WixUIExtension -out "%DEVROOT%\out\pkg\%PKGNAME%-%AERES_VERSION%.%TARGET_CPU%.msi" %PKGNAME%.wixobj

del aeres.wixobj

goto end

:help

echo "Usage: build.cmd <x86|x64>"

:end
