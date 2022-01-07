# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
set (EXTRA_COMPONENT_DIRS $ENV "/opt/esp/componentes")

pdroject(iotOnOff)
#add_custom_target( ALL NAME prueba COMMAND date)
set (SERVIDOR_OTA jajicaiot.ddns.net)
set (PUERTO_OTA 8070)
set (DIR_REPOSITORIO /opt/trastero/servidorWeb)
set (DIR_INSTALL firmware)
set (PREFIJO_OTA newVersion)
set (DIR_LASTVERSION lastVersion)
set (URL /firmware/${PROJECT_NAME}/)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

string(TIMESTAMP variable "%y%m%d%H%M")


set(PROJECT_VER ${variable})

target_add_binary_data(mqtt_ssl.elf "main/ca.crt" TEXT)

#add_custom_target( instalar ALL COMMAND 
#python ${PROJECT_DIR}/instalar.py ${SERVIDOR_OTA} ${PUERTO_OTA} ${PROJECT_NAME} ${DIR_REPOSITORIO} 
#${BUILD_DIR} ${PROJECT_VER} ${PREFIJO_OTA} ${DIR_LASTVERSION} ${URL} DEPENDS app)
