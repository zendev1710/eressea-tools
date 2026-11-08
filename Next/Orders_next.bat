@echo off
setlocal
pushd "%~dp0"

set "ROOT=%~dp0.."
set "INPUT=%~dp0ngm.cr"
set "HIERARCHY=%~dp0eressea.crh"
set "OUTPUT_DIR=%~dp0..\Next2"
set "CHECKORDERS=%ROOT%\CheckOrders\Release\CheckOrders.exe"

if not exist "%CHECKORDERS%" (
  echo CheckOrders.exe not found. Build the Release Win32 solution first.
  goto :fail
)
if not exist "%INPUT%" (
  echo Missing private input report: ngm.cr
  goto :fail
)
if not exist "%HIERARCHY%" (
  echo Missing private hierarchy file: eressea.crh
  goto :fail
)
if not defined ECHECKPATH (
  echo Set ECHECKPATH to the folder containing the CheckOrders locale data.
  goto :fail
)
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"
if errorlevel 1 goto :fail

"%CHECKORDERS%" -X -l -C -w6 -O "%~dp0result.txt" -S -f "%INPUT%" -H "%HIERARCHY%" -P "%ECHECKPATH%" -R "%OUTPUT_DIR%\ngm.cr" -i "%~dp0"
set "RC=%ERRORLEVEL%"
if not "%RC%"=="0" goto :done
where uedit32.exe >nul 2>nul
if not errorlevel 1 uedit32.exe "%~dp0result.txt"

:done
popd
exit /b %RC%

:fail
set "RC=%ERRORLEVEL%"
if "%RC%"=="0" set "RC=1"
popd
exit /b %RC%
