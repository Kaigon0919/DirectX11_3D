
cd..
cd..

xcopy .\D3DEngine\Include\*.h .\Engine\Include\ /d /y /s
xcopy .\D3DEngine\Bin\*.* .\Engine\Bin\ /d /y /s /exclude:exclude.txt
xcopy .\D3DEngine\Bin\*.* .\Client\Bin\ /d /y /s /exclude:exclude.txt
xcopy .\D3DEngine\Bin\*.* .\Editor\Bin\ /d /y /s /exclude:exclude.txt