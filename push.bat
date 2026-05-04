@echo off
where git >nul 2>&1
if errorlevel 1 (
  echo Git not found in PATH. Install Git and re-run this script.
  pause
  exit /b 1
)
cd /d "%~dp0"
git init
git add README.md
git add .
git commit -m "first commit" || echo No changes to commit.
git branch -M main
REM remove existing origin if present
git remote remove origin 2>nul
git remote add origin https://github.com/MohamedTurb/library-management-system-.git
git push -u origin main
pause
