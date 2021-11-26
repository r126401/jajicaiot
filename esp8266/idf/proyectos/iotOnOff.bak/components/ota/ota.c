/* OTA example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


//#include <sys/socket.h>
#include <netdb.h>


#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvslib.h"
#include "nvs_flash.h"
#include "esp_ota_ops.h"
#include "configuracion_usuario.h"
#include "ota.h"
#include "configuracion.h"
#include "api_json.h"


//#define EXAMPLE_SERVER_IP   CONFIG_SERVER_IP
//#define EXAMPLE_SERVER_PORT CONFIG_SERVER_PORT
//#define EXAMPLE_FILENAME CONFIG_EXAMPLE_FILENAME
#define BUFFSIZE 1500
#define TEXT_BUFFSIZE 1024

#define PETICION_HTTP "Connection: keep-alive\r\n\
Cache-Control: no-cache\r\n\
User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.101 Safari/537.36 \r\n\
Accept: */*\r\n\
Accept-Encoding: gzip,deflate,sdch\r\n\
Accept-Language: zh-CN,zh,es;q=0.8\r\n\r\n"

typedef enum esp_ota_firm_state {
    ESP_OTA_INIT = 0,
    ESP_OTA_PREPARE,
    ESP_OTA_START,
    ESP_OTA_RECVED,
    ESP_OTA_FINISH,
} esp_ota_firm_state_t;

typedef struct esp_ota_firm {
    uint8_t             ota_num;
    uint8_t             update_ota_num;

    esp_ota_firm_state_t    state;

    size_t              content_len;

    size_t              read_bytes;
    size_t              write_bytes;

    size_t              ota_size;
    size_t              ota_offset;

    const char          *buf;
    size_t              bytes;
} esp_ota_firm_t;

static const char *TAG = "OTA";
/*an ota data write buffer ready to write to the flash*/
static char ota_write_data[BUFFSIZE + 1] = { 0 };
/*an packet receive buffer*/
static char text[BUFFSIZE + 1] = { 0 };
/* an image total length*/
static int binary_file_length = 0;
/*socket id*/
//static int socket_id = -1;


static uint8_t get_ota_partition_count(void)
{
    uint16_t ota_app_count = 0;
    while (esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_OTA_MIN + ota_app_count, NULL) != NULL) {
            assert(ota_app_count < 16 && "must erase the partition before writing to it");
            ota_app_count++;
    }
    return ota_app_count;
}

/*read buffer by byte still delim ,return read bytes counts*/
static int read_until(const char *buffer, char delim, int len)
{
//  /*TODO: delim check,buffer check,further: do an buffer length limited*/
    int i = 0;
    while (buffer[i] != delim && i < len) {
        ++i;
    }
    return i + 1;
}


void task_fatal_error(DATOS_APLICACION *datosApp, ESTADO_OTA estado) {

	if (datosApp == NULL) return;
	notificar_evento_ota(datosApp, estado);
	esp_restart();
}

bool _esp_ota_firm_parse_http(esp_ota_firm_t *ota_firm, const char *text, size_t total_len, size_t *parse_len)
{
    /* i means current position */
    int i = 0, i_read_len = 0;
    char *ptr = NULL, *ptr2 = NULL;
    char length_str[32];
    int tipo_memoria = CONFIG_MEMORIA_ESP8266;

    while (text[i] != 0 && i < total_len) {
        if (ota_firm->content_len == 0 && (ptr = (char *)strstr(text, "Content-Length")) != NULL) {
            ptr += 16;
            ptr2 = (char *)strstr(ptr, "\r\n");
            memset(length_str, 0, sizeof(length_str));
            memcpy(length_str, ptr, ptr2 - ptr);
            ota_firm->content_len = atoi(length_str);
            if (tipo_memoria == 0) {
            	ota_firm->ota_size = ota_firm->content_len / ota_firm->ota_num;
            	ota_firm->ota_offset = ota_firm->ota_size * ota_firm->update_ota_num;
            } else {
                ota_firm->ota_size = ota_firm->content_len;
                ota_firm->ota_offset = 0;

            }
            ESP_LOGI(TAG, ""TRAZAR"parse Content-Length:%d, ota_size %d y tipo de Memoria %d", INFOTRAZA, ota_firm->content_len, ota_firm->ota_size, tipo_memoria);
        }

        i_read_len = read_until(&text[i], '\n', total_len - i);

        if (i_read_len > total_len - i) {
            ESP_LOGE(TAG, ""TRAZAR"recv malformed http header", INFOTRAZA);
            task_fatal_error(NULL, OTA_CABECERA_MAL_FORMADA);
        }

        // if resolve \r\n line, http header is finished
        if (i_read_len == 2) {
            if (ota_firm->content_len == 0) {
                ESP_LOGE(TAG, ""TRAZAR"did not parse Content-Length item", INFOTRAZA);
                task_fatal_error(NULL, OTA_CABECERA_MAL_FORMADA);
            }

            *parse_len = i + 2;

            return true;
        }

        i += i_read_len;
    }

    return false;
}

static size_t esp_ota_firm_do_parse_msg(esp_ota_firm_t *ota_firm, const char *in_buf, size_t in_len)
{
    size_t tmp;
    size_t parsed_bytes = in_len; 

    switch (ota_firm->state) {
        case ESP_OTA_INIT:
            if (_esp_ota_firm_parse_http(ota_firm, in_buf, in_len, &tmp)) {
                ota_firm->state = ESP_OTA_PREPARE;
                ESP_LOGD(TAG, "Http parse %d bytes", tmp);
                parsed_bytes = tmp;
            }
            break;
        case ESP_OTA_PREPARE:
            ota_firm->read_bytes += in_len;

            if (ota_firm->read_bytes >= ota_firm->ota_offset) {
                ota_firm->buf = &in_buf[in_len - (ota_firm->read_bytes - ota_firm->ota_offset)];
                ota_firm->bytes = ota_firm->read_bytes - ota_firm->ota_offset;
                ota_firm->write_bytes += ota_firm->read_bytes - ota_firm->ota_offset;
                ota_firm->state = ESP_OTA_START;
                ESP_LOGD(TAG, "Receive %d bytes and start to update", ota_firm->read_bytes);
                ESP_LOGD(TAG, "Write %d total %d", ota_firm->bytes, ota_firm->write_bytes);
            }

            break;
        case ESP_OTA_START:
            if (ota_firm->write_bytes + in_len > ota_firm->ota_size) {
                ota_firm->bytes = ota_firm->ota_size - ota_firm->write_bytes;
                ota_firm->state = ESP_OTA_RECVED;
            } else
                ota_firm->bytes = in_len;

            ota_firm->buf = in_buf;

            ota_firm->write_bytes += ota_firm->bytes;

            ESP_LOGD(TAG, "Write %d total %d", ota_firm->bytes, ota_firm->write_bytes);

            break;
        case ESP_OTA_RECVED:
            parsed_bytes = 0;
            ota_firm->state = ESP_OTA_FINISH;
            break;
        default:
            parsed_bytes = 0;
            ESP_LOGI(TAG, "State is %d", ota_firm->state);
            break;
    }

    return parsed_bytes;
}

static void esp_ota_firm_parse_msg(esp_ota_firm_t *ota_firm, const char *in_buf, size_t in_len)
{
    size_t parse_bytes = 0;

    //ESP_LOGI(TAG, ""TRAZAR"Input %d bytes", INFOTRAZA, in_len);


    do {
        size_t bytes = esp_ota_firm_do_parse_msg(ota_firm, in_buf + parse_bytes, in_len - parse_bytes);
        ESP_LOGD(TAG, ""TRAZAR"Parse %d bytes", INFOTRAZA, bytes);
        if (bytes)
            parse_bytes += bytes;
    } while (parse_bytes != in_len);
}

static inline int esp_ota_firm_is_finished(esp_ota_firm_t *ota_firm)
{
    return (ota_firm->state == ESP_OTA_FINISH || ota_firm->state == ESP_OTA_RECVED);
}

static inline int esp_ota_firm_can_write(esp_ota_firm_t *ota_firm)
{
    return (ota_firm->state == ESP_OTA_START || ota_firm->state == ESP_OTA_RECVED);
}

static inline const char* esp_ota_firm_get_write_buf(esp_ota_firm_t *ota_firm)
{
    return ota_firm->buf;
}

static inline size_t esp_ota_firm_get_write_bytes(esp_ota_firm_t *ota_firm)
{
    return ota_firm->bytes;
}

static void esp_ota_firm_init(esp_ota_firm_t *ota_firm, const esp_partition_t *update_partition)
{
    memset(ota_firm, 0, sizeof(esp_ota_firm_t));
    ota_firm->state = ESP_OTA_INIT;
    ota_firm->ota_num = get_ota_partition_count();
    ota_firm->update_ota_num = update_partition->subtype - ESP_PARTITION_SUBTYPE_APP_OTA_0;

    ESP_LOGI(TAG, ""TRAZAR"Totoal OTA number %d update to %d part", INFOTRAZA, ota_firm->ota_num, ota_firm->update_ota_num);

}

void tarea_upgrade_firmware(DATOS_APLICACION *datosApp)
{

    esp_err_t err;
    int conexion_http;
    /* update handle : set by esp_ota_begin(), must be freed via esp_ota_end() */
    esp_ota_handle_t update_handle = 0 ;
    const esp_partition_t *update_partition = NULL;

    ESP_LOGI(TAG, ""TRAZAR"comienzo de tarea OTA... @ %p flash %s", INFOTRAZA, __func__, CONFIG_ESPTOOLPY_FLASHSIZE);

    const esp_partition_t *configured = esp_ota_get_boot_partition();
    const esp_partition_t *running = esp_ota_get_running_partition();

    if (configured != running) {
    	ESP_LOGW(TAG, ""TRAZAR"OTA particion boor configurada en offset 0x%08x pero corriendo en offset 0x%08x", INFOTRAZA,
    			configured->address, running->address);
        ESP_LOGW(TAG, ""TRAZAR"Esto puede suceder si los datos de inicio de OTA o la imagen de inicio preferida se corrompen de alguna manera", INFOTRAZA);

    }

    ESP_LOGI(TAG, ""TRAZAR"Corriendo la particion tipo %d subtipo %d (offset 0x%08x)", INFOTRAZA,
             running->type, running->subtype, running->address);

    if ((conexion_http = conexion_servidor_http(datosApp)) < 0) {
    	ESP_LOGE(TAG, ""TRAZAR"ERROR AL CREAR LA CONEXION CON EL SERVIDOR OTA", INFOTRAZA);
    	return;
    }

    /*send GET request to http server*/
    const char *GET_FORMAT =
        "GET %s%s HTTP/1.0\r\n"
        "Host: %s:%d\r\n" \
        PETICION_HTTP;

    char *http_request = NULL;
    //int get_len = asprintf(&http_request, GET_FORMAT, EXAMPLE_FILENAME, EXAMPLE_SERVER_IP, EXAMPLE_SERVER_PORT)
    int get_len = asprintf(&http_request, GET_FORMAT, datosApp->datosGenerales->ota.url,
    		datosApp->datosGenerales->ota.file, datosApp->datosGenerales->ota.server,
			datosApp->datosGenerales->ota.puerto);
    ESP_LOGI(TAG, ""TRAZAR"PETICION: %s %d", INFOTRAZA, http_request, get_len);
    if (get_len < 0) {
        ESP_LOGE(TAG, ""TRAZAR"Failed to allocate memory for GET request buffer", INFOTRAZA);
        task_fatal_error(datosApp, OTA_ERROR_MEMORIA_DISPONIBLE);
    }

    if (write(conexion_http, http_request, strlen(http_request)) < 0) {
        ESP_LOGE(TAG, ""TRAZAR"... socket send failed", INFOTRAZA);
        close(conexion_http);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
    }
    ESP_LOGI(TAG, ""TRAZAR"... socket send success", INFOTRAZA);

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 120;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(conexion_http, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
            sizeof(receiving_timeout)) < 0) {
        ESP_LOGE(TAG, ""TRAZAR"... failed to set socket receiving timeout", INFOTRAZA);
        close(conexion_http);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
        return;
    }
    ESP_LOGI(TAG, ""TRAZAR"... set socket receiving timeout success", INFOTRAZA);


    update_partition = esp_ota_get_next_update_partition(NULL);
    ESP_LOGI(TAG, ""TRAZAR"Writing to partition subtype %d at offset 0x%x", INFOTRAZA,
             update_partition->subtype, update_partition->address);
    assert(update_partition != NULL);

    notificar_evento_ota(datosApp, OTA_DESCARGA_FIRMWARE);
    err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, ""TRAZAR"esp_ota_begin failed, error=%d", INFOTRAZA, err);
        task_fatal_error(datosApp, OTA_ERROR_OTA_BEGIN);
    }
    ESP_LOGI(TAG, ""TRAZAR"esp_ota_begin succeeded", INFOTRAZA);

    bool flag = true;
    esp_ota_firm_t ota_firm;

    esp_ota_firm_init(&ota_firm, update_partition);

    notificar_evento_ota(datosApp, OTA_COPIANDO_SECTORES);
    /*deal with all receive packet*/
    ESP_LOGI(TAG, "COMIENZA EL UPDATE");
    while (flag) {
        memset(text, 0, TEXT_BUFFSIZE);
        memset(ota_write_data, 0, BUFFSIZE);
        //int buff_len = read(conexion_http, text, TEXT_BUFFSIZE);

        int buff_len = recv(conexion_http, text, TEXT_BUFFSIZE, MSG_MORE);
        //ESP_LOGI(TAG, ""TRAZAR"RECIBIENDO %d: %s datos", INFOTRAZA, buff_len, text);

        if (buff_len < 0) { /*receive error*/
            ESP_LOGE(TAG, ""TRAZAR"Error: receive data error! errno=%d", INFOTRAZA, errno);
            task_fatal_error(datosApp, OTA_PAQUETES_ERRONEOS);
        } else if (buff_len > 0) { /*deal with response body*/

            esp_ota_firm_parse_msg(&ota_firm, text, buff_len);

            if (!esp_ota_firm_can_write(&ota_firm)) {
            	ESP_LOGI(TAG, ""TRAZAR"AUN NO PUEDO ESCRIBIR", INFOTRAZA);
            	continue;
            }

            memcpy(ota_write_data, esp_ota_firm_get_write_buf(&ota_firm), esp_ota_firm_get_write_bytes(&ota_firm));
            buff_len = esp_ota_firm_get_write_bytes(&ota_firm);

            err = esp_ota_write( update_handle, (const void *)ota_write_data, buff_len);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, ""TRAZAR"Error: esp_ota_write failed! err=0x%x", INFOTRAZA, err);
                task_fatal_error(datosApp, OTA_ERROR_AL_ESCRIBIR_EN_MEMORIA);
            }
            binary_file_length += buff_len;
            ESP_LOGD(TAG, ""TRAZAR"Have written image length %d", INFOTRAZA, binary_file_length);
        } else if (buff_len == 0) {  /*packet over*/
            flag = false;
            ESP_LOGI(TAG, ""TRAZAR"Connection closed, all packets received", INFOTRAZA);
            close(conexion_http);
        } else {
            ESP_LOGE(TAG, ""TRAZAR"Unexpected recv result", INFOTRAZA);
        }

        if (esp_ota_firm_is_finished(&ota_firm))
            break;
    }

    ESP_LOGI(TAG, ""TRAZAR"Total Write binary data length : %d", INFOTRAZA, binary_file_length);

    if (esp_ota_end(update_handle) != ESP_OK) {
        ESP_LOGE(TAG, ""TRAZAR"esp_ota_end failed!", INFOTRAZA);
        task_fatal_error(datosApp, OTA_CRC_ERRONEO);
    }
    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, ""TRAZAR"esp_ota_set_boot_partition failed! err=0x%x", INFOTRAZA, err);
        task_fatal_error(datosApp, OTA_ERROR_PARTICION);
    }
    notificar_evento_ota(datosApp, OTA_UPGRADE_FINALIZADO);
    ESP_LOGI(TAG, ""TRAZAR"Prepare to restart system!", INFOTRAZA);
    salvar_configuracion_general(datosApp);
    ESP_LOGE(TAG, ""TRAZAR" VAMOS A REINICIAR", INFOTRAZA);

    esp_restart();
    return ;
}

int conexion_servidor_http(DATOS_APLICACION *datosApp) {

    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s;
    char puerto[6] = {0};
    itoa(datosApp->datosGenerales->ota.puerto, puerto, 10);

    int err = getaddrinfo(datosApp->datosGenerales->ota.server, puerto, &hints, &res);

     if(err != 0 || res == NULL) {
         ESP_LOGE(TAG, ""TRAZAR"DNS lookup error err=%d res=%p", INFOTRAZA, err, res);
         vTaskDelay(1000 / portTICK_PERIOD_MS);
         return ESP_FAIL;
     }

     /* Code to print the resolved IP.

        Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
     addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
     ESP_LOGI(TAG, ""TRAZAR"DNS lookup con exito. IP=%s", INFOTRAZA, inet_ntoa(*addr));
     s = socket(res->ai_family, res->ai_socktype, 0);
     if(s < 0) {
         ESP_LOGE(TAG, ""TRAZAR"... Fallo al crear el socket.", INFOTRAZA);
         freeaddrinfo(res);
         vTaskDelay(1000 / portTICK_PERIOD_MS);
         return ESP_FAIL;
     }
     ESP_LOGI(TAG, ""TRAZAR"... Socket creado", INFOTRAZA);

     if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
         ESP_LOGE(TAG, ""TRAZAR"... Error al conectarse al servidor errno=%d", INFOTRAZA, errno);
         close(s);
         freeaddrinfo(res);
         vTaskDelay(4000 / portTICK_PERIOD_MS);
         return ESP_FAIL;
     }

     ESP_LOGI(TAG, ""TRAZAR"... Conectado!", INFOTRAZA);
     freeaddrinfo(res);



	return s;
}

