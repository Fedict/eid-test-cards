
:: set all path variables
:: =====================
@call "%~dp0.\SetPaths.bat"
@if %ERRORLEVEL%==1 goto paths_failed

:: build pcscproxy and pcsctool
:: ====================================================
@echo [INFO] Building "%~dp0.\eidtest.sln"
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /p:VisualStudioVersion=14.0 /target:clean /property:Configuration=Release /Property:Platform=Win32 %~dp0.\eidtest.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /p:VisualStudioVersion=14.0 /target:build /property:Configuration=Release /Property:Platform=Win32 "%~dp0.\eidtest.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /p:VisualStudioVersion=14.0 /target:clean /property:Configuration=Release /Property:Platform=x64 "%~dp0.\eidtest.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed
@"%BEID_DIR_MSBUILD%\MSBuild.exe" /p:VisualStudioVersion=14.0 /target:build /property:Configuration=Release /Property:Platform=x64 "%~dp0.\eidtest.sln"
@if "%ERRORLEVEL%" == "1" goto msbuild_failed

:: build pcsccontrol
:: ==================================
set OUR_CURRENT_PATH="%cd%"
cd "%~dp0..\pcsccontrol"
call .\build.bat
@goto end_resetpath

:msbuild_failed
@echo [ERR ] msbuild failed
@goto err

:paths_failed
@echo [ERR ] could not set paths
@goto err

:end_resetpath
@cd %OUR_CURRENT_PATH%

@echo [INFO] Build_all Done...
@goto end

:err
@exit /b 1

:end

