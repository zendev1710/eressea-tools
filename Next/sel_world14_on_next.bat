@echo off
setlocal
pushd "%~dp0"

set "ROOT=%~dp0.."
set "INPUT=%~dp0ngm.cr"
set "OUTPUT=%~dp0ngm_world14.cr"
set "HIERARCHY=%~dp0eressea.crh"
set "SELECTION=%~dp0world14.sel"
set "CRSAVE=%ROOT%\CRSel\Release\CRSave.exe"

if not exist "%CRSAVE%" (
  echo CRSave.exe not found. Build the Release Win32 solution first.
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
if not exist "%SELECTION%" (
  echo Missing private selection file: world14.sel
  goto :fail
)

"%CRSAVE%" -f "%INPUT%" -l "%SELECTION%" -H "%HIERARCHY%" -S "%OUTPUT%" -x kerubin -g
set "RC=%ERRORLEVEL%"
popd
exit /b %RC%

:fail
set "RC=%ERRORLEVEL%"
if "%RC%"=="0" set "RC=1"
popd
exit /b %RC%
