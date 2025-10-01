@echo off
setlocal EnableDelayedExpansion

:: Name of the output executable
set OUTPUT=gigatron.exe

:: List of all .c source files
set SOURCES=main.c src\core.c src\keyboard.c src\audio.c

set INCLUDE_PATH=.\include
set LIB_PATH=.\
:: Compiler flags (you can add -Wall -O2 etc.)
set CFLAGS=-I%INCLUDE_PATH% -Wall -Werror -O2

:: Linker flags (libraries to link)
set LIBS=-L%LIB_PATH% -lglfw3 -lopengl32 -lglu32 -lgdi32 -lwinmm

:: Build

echo Compiling...
gcc %CFLAGS% %SOURCES% %LIBS% -o %OUTPUT%

if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

echo Build succeeded. Output: %OUTPUT%
