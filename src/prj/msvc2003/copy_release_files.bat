xcopy /y /E ..\..\..\3pl\windows\vc2003\lib\Release\*.dll .\bin\Release\

xcopy /y /E %OGRE_HOME%\bin\release\*.dll .\bin\release

xcopy /y ..\..\*Config.xml .\bin\Release
xcopy /y ..\..\*Config.xsd .\bin\Release


mkdir .\bin\data
xcopy /y /E ..\..\..\data .\bin\data


mkdir .\bin\cfg
xcopy /y /E ..\..\..\cfg .\bin\cfg

REM allow local overrides in the project directory
REM

xcopy /y .\*Config.xml .\bin\cfg