xcopy /y /E ..\..\..\3pl\windows\vc2003\lib\Debug\*.dll .\bin\Debug\

xcopy /y /E %OGRE_HOME%\bin\debug\*.dll .\bin\debug

xcopy /y ..\..\*Config.xml .\bin\Debug
xcopy /y ..\..\*Config.xsd .\bin\Debug



mkdir .\bin\data
xcopy /y /E ..\..\..\data .\bin\data

mkdir .\bin\cfg
xcopy /y /E ..\..\..\cfg .\bin\cfg

REM allow local overrides in the project directory
REM