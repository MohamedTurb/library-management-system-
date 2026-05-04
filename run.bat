@echo off
rem run.bat - Launch the built LibraryManagementSystem.exe from the build folder

pushd "%~dp0build"
if not exist "LibraryManagementSystem.exe" (
  echo Executable not found in %CD%\build
  popd
  exit /b 1
)

start "LibraryManagementSystem" "LibraryManagementSystem.exe"
popd
exit /b 0
