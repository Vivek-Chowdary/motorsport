xcopy /y /E ..\..\..\3pl\windows\vc2003\lib\Release\*.dll .\bin\Release\

xcopy /y ..\..\*Config.xml .\bin\Release
xcopy /y ..\..\*Config.xsd .\bin\Release

mkdir .\bin\data
xcopy /y /E ..\..\..\data .\bin\data

