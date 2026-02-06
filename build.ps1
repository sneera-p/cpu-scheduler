# Simple build for console app
$CC = "clang-cl"
$SRC = @(".\src\*.c", ".\lib\pcg\*.c")
$INC = @("-I.\include", "-I.\lib")
$OUT = ".\bin\exe.exe"

& $CC $INC $SRC -Fe:$OUT /std:c23 /W4 /Zi /D_CRT_SECURE_NO_WARNINGS
