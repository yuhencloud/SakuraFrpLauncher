set QTDIR=.\QT_Build\bin
set RUNDIR=..\..\..\bin

xcopy /i /y %QTDIR%\Qt5Core.dll %RUNDIR%\Release\
xcopy /i /y %QTDIR%\Qt5Cored.dll %RUNDIR%\Debug\

xcopy /i /y %QTDIR%\Qt5Gui.dll %RUNDIR%\Release\
xcopy /i /y %QTDIR%\Qt5Guid.dll %RUNDIR%\Debug\

xcopy /i /y %QTDIR%\Qt5Widgets.dll %RUNDIR%\Release\
xcopy /i /y %QTDIR%\Qt5Widgetsd.dll %RUNDIR%\Debug\

xcopy /i /y %QTDIR%\Qt5Sql.dll %RUNDIR%\Release\
xcopy /i /y %QTDIR%\Qt5Sqld.dll %RUNDIR%\Debug\

xcopy /i /y %QTDIR%\Qt5Network.dll %RUNDIR%\Release\
xcopy /i /y %QTDIR%\Qt5Networkd.dll %RUNDIR%\Debug\

xcopy /i /y %QTDIR%\icuin54.dll %RUNDIR%\Release\
xcopy /i /y %QTDIR%\icuuc54.dll %RUNDIR%\Release\
xcopy /i /y %QTDIR%\icudt54.dll %RUNDIR%\Release\

xcopy /i /y %QTDIR%\icuin54.dll %RUNDIR%\Debug\
xcopy /i /y %QTDIR%\icuuc54.dll %RUNDIR%\Debug\
xcopy /i /y %QTDIR%\icudt54.dll %RUNDIR%\Debug\

pause