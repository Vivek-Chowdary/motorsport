xcopy /y /E ..\..\..\3pl\windows\vc2003\lib\Release\*.dll .\bin\Release\
xcopy /y ..\..\..\3pl\windows\vc2003\lib\xerces-c_2_5_0.dll .\bin\Release\

xcopy /y ..\..\*Config.xml .\bin\Release

mkdir .\bin\data
xcopy /y /E ..\..\..\data .\bin\data

