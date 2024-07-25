cd %~dp0

xcopy /y /q .\External\x64-windows\bin\*.dll .\Output\x64-Release

xcopy /y /q .\External\x64-windows\bin\*.pdb .\Output\x64-Release

xcopy /y /q .\External\x64-windows\debug\bin\*.dll .\Output\x64-Debug

xcopy /y /q .\External\x64-windows\debug\bin\*.pdb .\Output\x64-Debug
