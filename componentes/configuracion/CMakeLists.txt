idf_component_register(SRCS "configuracion.c" INCLUDE_DIRS "include" 
REQUIRES alarmas
api_json
comunes
conexiones
nvslib
interfaz_usuario
espota
programador EMBED_TXTFILES certs/mqtt_cert.crt)



