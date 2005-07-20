xcopy /y /E ..\..\..\3pl\windows\vc2003\lib\Debug\*.dll .\bin\Debug\

xcopy /y ..\..\*Config.xml .\bin\Debug
xcopy /y ..\..\*Config.xsd .\bin\Debug

REM allow local overrides in the project directory
REM

xcopy /y .\*Config.xml .\bin\Debug

mkdir .\bin\data
xcopy /y /E ..\..\..\data .\bin\data

