idf_component_register(SRCS "espota.c" INCLUDE_DIRS "include" 
EMBED_TXTFILES ${project_dir}/certificado_ota/ca_cert.pem
REQUIRES alarmas
api_json
comunes
conexiones
nvslib
configuracion
programador
main)

