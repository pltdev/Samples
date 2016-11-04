:: Build script for project innovate-ncdemo.

@echo off
setlocal

:: Display usage.
if "%1" == "clean" goto :nousage 
if "%1" == "release" goto :nousage 
  echo usage: build.bat [clean^|release]
  exit /b 1
:nousage

:: Verify that we are in the correct (top) directory.
if exist PLTLayer/PLTLayer.sln goto :okdir
echo ERROR - must execute from top level directory
goto :failure
:okdir

:: Location of tools.
set msbuild="C:\Program Files (x86)\MSBuild\12.0\Bin\MSBuild.exe" 

:: Clean Workspace.
echo Cleaning Workspace...
rmdir /s /q "PLTLayer\PLTLayer\obj" 2>nul:
rmdir /s /q "PLTLayerTestApp\obj" 2>nul:
rmdir /s /q "PLTLayer\PLTLayer\bin" 2>nul:
rmdir /s /q "PLTLayerTestApp\bin" 2>nul:
del /f /q "PLTLayerTestApp\PLTLayer.*" 2>nul:
del /f /q "PLTLayerTestApp\*.csv" 2>nul:
del /f /q "PLTLayerTestApp\*.py" 2>nul:
if "%1" == "clean" goto :success

:: Build Release. 
echo Building release...
set projA=/p:Configuration=Release PLTLayer\PLTLayer.sln 
set projB=/p:Configuration=Release PLTLayerTestApp\PLTLayerTestApp.sln 
call %msbuild% %projA% || goto :failure
call %msbuild% %projB% || goto :failure
if "%1" == "release" goto :success

:: Success.
:success
echo Success.
exit /b 0

:: Failure 
:failure
echo Failure
exit /b 1

