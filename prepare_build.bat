@ECHO OFF
if not exist C:\Software\Qt5.5.1\5.5\msvc2013 goto error

if not exist QT_Build mklink /d QT_Build C:\Software\Qt5.5.1\5.5\msvc2013

pause

exit /b 0

:error
echo No directory found
exit /b 1