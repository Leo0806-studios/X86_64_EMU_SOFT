set SrcFile=%1
set OutFile=%2
set BinType=%3
echo assembling %SrcFile%
nasm -f %BinType% %SrcFile% -o %OutFile%
if %ERRORLEVEL% NEQ 0 (
    echo error: NASM Compilation Failed for %SrcFile%!
    exit /b %ERRORLEVEL%
)

echo NASM Compilation Successful!
exit /b 0