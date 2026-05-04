# Build script for LibraryManagementSystem
# This script compiles the project using g++ directly

$projectDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $projectDir

$gcc = "C:\Users\Dell\scoop\apps\gcc\current\bin\g++.exe"
$moc = "C:\Users\Dell\Qt\6.6.3\mingw_64\bin\moc.exe"

# Compilation flags (from Makefile.Release)
$defines = "-DUNICODE -D_UNICODE -DWIN32 -DMINGW_HAS_SECURE_API=1 -DQT_NO_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_NEEDS_QMAIN"
$cxxflags = "-fno-keep-inline-dllexport -O2 -std=gnu++1z -Wall -Wextra -fexceptions -mthreads"
$incpath = "-I. -I`"C:\Users\Dell\Qt\6.6.3\mingw_64\include`" -I`"C:\Users\Dell\Qt\6.6.3\mingw_64\include\QtWidgets`" -I`"C:\Users\Dell\Qt\6.6.3\mingw_64\include\QtGui`" -I`"C:\Users\Dell\Qt\6.6.3\mingw_64\include\QtCore`""

# Libraries (from Makefile.Release)
$libs = "`"C:\Users\Dell\Qt\6.6.3\mingw_64\lib\libQt6Widgets.a`" `"C:\Users\Dell\Qt\6.6.3\mingw_64\lib\libQt6Gui.a`" `"C:\Users\Dell\Qt\6.6.3\mingw_64\lib\libQt6Core.a`" -lmingw32 `"C:\Users\Dell\Qt\6.6.3\mingw_64\lib\libQt6EntryPoint.a`" -lshell32"
$lflags = "-Wl,-s -Wl,-subsystem,windows -mthreads"

# Create build directory if it doesn't exist
if (!(Test-Path "release")) {
    New-Item -ItemType Directory -Name "release" -Force | Out-Null
}

Write-Host "Building LibraryManagementSystem..." -ForegroundColor Green

# Step 1: Generate moc file
Write-Host "Generating moc file..."
& $moc -o "release/moc_mainwindow.cpp" mainwindow.h
if ($LASTEXITCODE -ne 0) {
    Write-Host "MOC generation failed!" -ForegroundColor Red
    exit 1
}

# Step 2: Compile source files
$files = @(
    "main.cpp",
    "mainwindow.cpp",
    "linkedlist.cpp",
    "queue.cpp",
    "stack.cpp",
    "release/moc_mainwindow.cpp"
)

foreach ($file in $files) {
    $outputFile = $file -replace '\.cpp$', '.o'
    $outputFile = "release/" + (Split-Path -Leaf $outputFile)
    
    Write-Host "Compiling $file -> $outputFile..."
    $compileCmd = "$gcc -c -fno-keep-inline-dllexport -O2 -std=gnu++1z -Wall -Wextra -fexceptions -mthreads " + `
                  "$defines $incpath -o `"$outputFile`" `"$file`""
    Invoke-Expression $compileCmd
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Compilation of $file failed!" -ForegroundColor Red
        exit 1
    }
}

# Step 3: Link object files
Write-Host "Linking..."
$objectFiles = @(
    "release/main.o",
    "release/mainwindow.o",
    "release/linkedlist.o",
    "release/queue.o",
    "release/stack.o",
    "release/moc_mainwindow.o"
)

$allLibs = $libs.Split(" ") | Where-Object { $_ -ne "" }

& $gcc -Wl,-s -Wl,-subsystem,windows -mthreads -o "release/LibraryManagementSystem.exe" @objectFiles @allLibs

if ($LASTEXITCODE -ne 0) {
    Write-Host "Linking failed!" -ForegroundColor Red
    exit 1
}

Write-Host "Build completed successfully!" -ForegroundColor Green
Write-Host "Executable: release/LibraryManagementSystem.exe" -ForegroundColor Cyan
