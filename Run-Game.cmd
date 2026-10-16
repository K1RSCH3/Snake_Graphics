@echo off
setlocal
pushd "%~dp0Finals_GRAPHICS\x64\Debug"
if errorlevel 1 goto missing
if not exist "Finals_GRAPHICS.exe" goto missing
if not exist "raylib.dll" goto missing
if not exist "glfw3.dll" goto missing
echo Starting CYBERSNEK. Use the arrow keys to play; Esc closes the game.
"Finals_GRAPHICS.exe"
set "gameExitCode=%errorlevel%"
popd
if "%gameExitCode%"=="0" exit /b 0
echo.
echo The game exited with code %gameExitCode%. Check the messages above.
pause
exit /b %gameExitCode%

:missing
echo The game executable or required DLLs are missing.
echo Open Snake_Graphics in VS Code and run the Build Snake Graphics task first.
pause
exit /b 1
