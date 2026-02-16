pio run -e ESP32-DevKitV4-OLED096 -t upload --upload-port COM7
desde una terminal de powershell ejecutar este comando de upload con el proyecto abierto

Opción 1: Flashear todo de una vez (Recomendado)

python -m esptool --port COM7 write_flash 0x0 "C:\Users\mc\Downloads\NerdMiner_v2-main\NerdMiner_v2-main\.pio\build\ESP32-DevKitV4-OLED096\ESP32-DevKitV4-OLED096_factory.bin"

Opción 2: Flashear firmware solo (si ya tiene bootloader)
python -m esptool --port COM7 write_flash 0x10000 "C:\Users\mc\Downloads\NerdMiner_v2-main\NerdMiner_v2-main\.pio\build\ESP32-DevKitV4-OLED096\firmware.bin"