@echo off
echo ==============================================
echo      Compiling Engine and Starting GUI...
echo ==============================================

:: Check if an existing engine.exe is locked by a running process
if exist engine.exe (
    del /f /q engine.exe 2>nul
    if exist engine.exe (
        echo [ERROR] engine.exe is locked. Close the Python game window first!
        pause
        exit /b 1
    )
)

echo Compiling engine.cpp with -O3 optimizations...
g++ -O3 -std=c++17 engine.cpp -o engine.exe -static -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive

if %ERRORLEVEL% equ 0 (
    echo [SUCCESS] Build complete! Starting the chessboard...
    echo ==============================================
    
    python gui.py
) else (
    echo.
    echo [ERROR] Compilation failed. Check the error messages above.
    pause
)