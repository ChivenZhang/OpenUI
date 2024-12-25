cd %~dp0

xcopy /y /q .\Output\x64-Release\vcpkg_installed\x64-windows-static-md\etc\fonts\ .\Output\x64-Release\fonts\

xcopy /y /q .\Output\x64-Release\vcpkg_installed\x64-windows-static-md\etc\fonts\ .\Output\x64-Debug\fonts\
