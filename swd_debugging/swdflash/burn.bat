@echo off
echo %0
echo %1
if "%1" == "" (
    echo "please input flash image name"
) else (
    python swdflash.py --bin="%1" --addr=0x0
)