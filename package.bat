REM set your sdk path
set SDK=C:\Source\nRF5_SDK_17.1.0_ddde560

REM NRF52832
set FAMILY=NRF52
set HWVERSION=52
set SDVERSION=0x101
set SOFTDEVICE=%SDK%\components\softdevice\s132\hex\s132_nrf52_7.2.0_softdevice.hex

REM set your app/bl path (nRF52832)
set APP=examples\ble_peripheral\ble_app_buttonless_dfu\pca10040\s132\ses\Output\Release\Exe\ble_app_buttonless_dfu_pca10040_s132.hex
set BOOTLOADER=examples\dfu\secure_bootloader\pca10040_s132_ble\ses\Output\Release\Exe\secure_bootloader_ble_s132_pca10040.hex

REM set your private key path
set PRIVATEKEY=private.key


REM make image for ROM FLASHING
mkdir Output
nrfutil settings generate --family %FAMILY% --application %APP% --application-version 1 --bootloader-version 1 --bl-settings-version 1 Output\settings.hex
mergehex --merge %SOFTDEVICE% %BOOTLOADER% --output Output\SD_BL.hex
mergehex --merge Output\SD_BL.hex %APP% --output Output\SD_BL_APP.hex
mergehex --merge Output\SD_BL_APP.hex Output\settings.hex --output Output\all.hex

del Output\SD_BL.hex
del Output\SD_BL_APP.hex
del Output\settings.hex

REM make image for OTA
nrfutil pkg generate --hw-version %HWVERSION% --application-version 1 --application %APP% --sd-req %SDVERSION% --key-file %PRIVATEKEY% Output\ota_app.zip
nrfutil pkg generate --hw-version %HWVERSION% --bootloader-version 1 --bootloader %BOOTLOADER% --application-version 1 --application %APP% --softdevice %SOFTDEVICE% --sd-id %SDVERSION% --sd-req %SDVERSION% --key-file %PRIVATEKEY% Output\ota_all.zip

