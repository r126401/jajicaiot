idf_component_register(SRCS "ota.c" INCLUDE_DIRS "include"
REQUIRES alarmas
api_json
comunes
conexiones
nvslib
interfaz_usuario
configuracion
programador
app_update)
