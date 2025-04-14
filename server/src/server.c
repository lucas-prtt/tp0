#include "server.h"

int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);
	char * port = malloc(sizeof("4444"));
	memcpy(port, "4444", sizeof("4444"));
	int server_fd = crearSocketServer(port);
	log_info(logger, "Servidor listo para recibir al cliente");
	parametrosAtencionThread * param = malloc(sizeof(param));
	param->ejemplo = 42; 
	esperarClientes(server_fd, atenderConThread, param);
	return EXIT_SUCCESS;
}

