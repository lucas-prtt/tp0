#include "client.h"
#include <commons/log.h>
#include <readline/readline.h>
int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* clave;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();
	if(logger == NULL){
		abort();
	}
	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"
	log_info(logger, "Soy un log!");

	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();
	if(config == NULL){
		abort();
	}
	clave = config_get_string_value(config, "CLAVE");
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");
	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	// Loggeamos el valor de config
	log_info(logger, "CLAVE:");
	log_info(logger, clave);
	log_info(logger, "IP y puerto");
	log_info(logger, ip);
	log_info(logger, puerto);

	/* ---------------- LEER DE CONSOLA ---------------- */

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = conectarSocketClient(ip, puerto);
	log_info(logger, "aa");
	
	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje(clave, conexion);
	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger;
	nuevo_logger = log_create("tp0.log", "tp0", true, LOG_LEVEL_INFO);
	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config;
	nuevo_config = config_create("cliente.config");
	return nuevo_config;
}

void leer_consola(t_log* logger)
{
	char* leido;

	// La primera te la dejo de yapa
	leido = readline("> ");

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío

	while (leido[0]!='\0'){
		log_info(logger, leido);
		free(leido);
		leido = readline("> ");
	}
	free(leido);
	// ¡No te olvides de liberar las lineas antes de regresar!

}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete;
	paquete = crear_paquete();
	leido = readline("> ");

	while (leido[0]!='\0'){
		agregar_a_paquete(paquete, leido, strlen(leido));
		free(leido);
		leido = readline("> ");
	}
	free(leido);

	// Leemos y esta vez agregamos las lineas al paquete
	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
}
