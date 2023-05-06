@echo OFF

SET SOURCE=https://dl.espressif.com/dl/
SET MSYS32=esp32_win32_msys2_environment_and_toolchain-20181001.zip
SET TOOLCHAIN=xtensa-lx106-elf-gcc8_4_0-esp-2020r3-win32.zip

if not exist ESP8266_RTOS_SDK\ (
echo Getting esp8266 rtos sdk
git clone --recursive https://github.com/espressif/ESP8266_RTOS_SDK ESP8266_RTOS_SDK
) else (
echo ESP8266_RTOS_SDK has already existed
)

if not exist msys32\ (
echo Downloading msys32 from %SOURCE%%MSYS32%
curl -# -O %SOURCE%%MSYS32%
echo Unzipping ...
tar -xzf %MSYS32%
echo Deleting zip ...
del /f %MSYS32%
) else (
echo Environment has already existed
)

if not exist xtensa-lx106-elf\ (
then
echo Downloading esp8266 toolchain from %SOURCE%%TOOLCHAIN%
curl -# -O %SOURCE%%TOOLCHAIN%
echo Unzipping ...
tar -xzf %TOOLCHAIN%
echo Deleting zip ...
del /f %TOOLCHAIN%
) else (
echo Toolchain has already existed
)

echo Done

pause