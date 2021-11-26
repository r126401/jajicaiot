/*
 * cronotermostato.h
 *
 *  Created on: 5 abr. 2021
 *      Author: t126401
 */

#ifndef LV_EXAMPLES_SRC_CRONOTERMOSTATO_LV_CRONOTERMOSTATO_H_
#define LV_EXAMPLES_SRC_CRONOTERMOSTATO_LV_CRONOTERMOSTATO_H_

/**
 * @fn void lv_pantalla_cronotermostato_app()
 * @brief Primera llamada de la aplicacion para encender el display.
 *
 */
void lv_pantalla_cronotermostato_app();

void lv_actualizar_estado_wifi_lcd(DATOS_APLICACION *datosApp, lv_color_t color);

#endif /* LV_EXAMPLES_SRC_CRONOTERMOSTATO_LV_CRONOTERMOSTATO_H_ */
