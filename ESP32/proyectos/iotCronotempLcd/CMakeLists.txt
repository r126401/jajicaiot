# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
set (EXTRA_COMPONENT_DIRS "/opt/esp/espidf/v4.4/componentes")
#set (EXTRA_COMPONENT_DIRS "../../../componentes/")
list(APPEND EXTRA_COMPONENT_DIRS components/lvgl_esp32_drivers components/lvgl_esp32_drivers/lvgl_touch components/lvgl_esp32_drivers/lvgl_tft)

#add_custom_target( ALL NAME prueba COMMAND date)
set (SERVIDOR_OTA jajicaiot.ddns.net)
set (PUERTO_OTA 8070)
set (DIR_REPOSITORIO /opt/trastero)
set (DIR_INSTALL firmware)
set (PREFIJO_OTA newVersion)
set (DIR_LASTVERSION lastVersion)
set (URL /firmware/${PROJECT_NAME}/)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

string(TIMESTAMP variable "%y%m%d%H%M")


set(PROJECT_VER ${variable})
project(iotCronotempLcd)
add_custom_target( instalar ALL COMMAND 
python ${PROJECT_DIR}/instalar.py ${SERVIDOR_OTA} ${PUERTO_OTA} ${PROJECT_NAME} ${DIR_REPOSITORIO} 
${BUILD_DIR} ${PROJECT_VER} ${PREFIJO_OTA} ${DIR_LASTVERSION} ${URL} DEPENDS app)





