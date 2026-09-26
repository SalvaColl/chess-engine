@echo off
echo ==============================================
echo        Building Standalone Chess Engine
echo ==============================================

:: Check if an existing engine.exe is locked by a running process
if exist engine.exe (
    del /f /q engine.exe 2>nul
    if exist engine.exe (
        echo [ERROR] engine.exe is locked. Close Cutechess or your GUI first!
        pause
        exit /b 1
    )
)

echo Compiling engine.cpp with -O3 optimizations...
g++ -O3 -std=c++17 engine.cpp -o engine.exe -static -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive

if %ERRORLEVEL% equ 0 (
    echo.
    echo [SUCCESS] engine.exe built successfully!
    echo File size:
    for %%I in (engine.exe) do echo %%~zI bytes
    
    :: Automatically copy to the PyInstaller dist folder if it exists
    if exist dist\gui\ (
        echo Copying to dist\gui...
        copy /y engine.exe dist\gui\engine.exe
        echo [SUCCESS] Engine copied to GUI folder!
    )
) else (
    echo.
    echo [ERROR] Compilation failed. Check the error messages above.
)

echo.
pause