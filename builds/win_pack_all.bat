rmdir /s /q "%~dp0..\_Package"
mkdir "%~dp0..\_Package"
mkdir "%~dp0..\_Package\1_gen_virtual_cards"
mkdir "%~dp0..\_Package\Documents"
mkdir "%~dp0..\_Package\2_PCSCControl"
mkdir "%~dp0..\_Package\3_PCSCProxy"
mkdir "%~dp0..\_Package\3_PCSCProxy\Win32"
mkdir "%~dp0..\_Package\3_PCSCProxy\x64"

:: copy virtual card generator
xcopy /s /e "%~dp0\..\gen_virtual_cards" "%~dp0..\_Package\1_gen_virtual_cards"

::copy documents folder
xcopy /s /e "%~dp0\..\documents" "%~dp0..\_Package\Documents"

::copy pcsccontrol
xcopy /s /e "%~dp0\..\PCSCControl" "%~dp0..\_Package\2_PCSCControl"

::copy pcscsmartcard 32 bit
xcopy "%~dp0\..\_Binaries\Win32\Release\winsc*" "%~dp0..\_Package\3_PCSCProxy\Win32\"
xcopy "%~dp0\..\..\ThirdParty\windows-installer\vc_redist.x86.exe" "%~dp0..\_Package\3_PCSCProxy\Win32\"

::copy pcscsmartcard 64 bit
xcopy "%~dp0\..\_Binaries\x64\Release\winsc*" "%~dp0..\_Package\3_PCSCProxy\x64\"
xcopy "%~dp0\..\..\ThirdParty\windows-installer\vc_redist.x64.exe" "%~dp0..\_Package\3_PCSCProxy\x64\"

::copy overal readme and license file
xcopy "%~dp0\..\installers\license.txt" "%~dp0..\_Package\"
xcopy "%~dp0\..\installers\windows\readme.txt" "%~dp0..\_Package\"

:end

