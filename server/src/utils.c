#include "utils.h"
#include <errno.h>


t_log* logger;
int crearSocket(char* ip, char* puerto, struct addrinfo * * server_info){
    int soc;
    struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // IGNORADA SI ip != NULL (en cliente)

	getaddrinfo(ip, puerto, &hints, server_info);
    soc = socket((*server_info)->ai_family, (*server_info)->ai_socktype, (*server_info)->ai_protocol);
    setsockopt(soc, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));
    return soc;
}

int crearSocketServer(char* puerto){
    int soc;
    struct addrinfo * serverInfo = NULL;
    soc = crearSocket(NULL, puerto, &serverInfo);
    bind(soc, serverInfo->ai_addr, serverInfo->ai_addrlen);
	listen(soc, SOMAXCONN);
	freeaddrinfo(serverInfo);
    return soc;
}

int esperarClientes(int socket_server, void (*atenderCliente)(void*), parametrosAtencionThread * params){ //EsperarClientes(): Se queda siempre en esta funcion la cual crea threads. Toma la funcion atenderCliente para crear el thread "
    logger = log_create("logthreads.log", "thread", true, LOG_LEVEL_INFO);
	while (1) {
        pthread_t thread;
		infoAtencionThread * infoThread;
		infoThread = malloc(sizeof(infoAtencionThread));
		infoThread->socket = accept(socket_server, NULL, NULL);
		infoThread->parametros = params;
		log_info(logger, "esperando clientes");
        pthread_create(&thread,
                    NULL,
                    atenderConThread,
                    (void*)infoThread);
        pthread_detach(thread);
    }
}

void * atenderConThread(void * infoThread){
	logger = log_create("logthreads.log", "thread", true, LOG_LEVEL_INFO);
	log_info(logger, "creado el thread");
	t_list* lista;

	int cliente_fd = (*(infoAtencionThread*)infoThread).socket;
	parametrosAtencionThread * parametros = (*(infoAtencionThread*)infoThread).parametros;

	while (1) {
		printf("El numero del dia es %d", parametros->ejemplo);
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			free(infoThread);
			pthread_exit(0);		
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
		free(infoThread);
	pthread_exit(0);
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}



int iniciar_servidor(void)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, PUERTO, &hints, &servinfo);
	
	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	// Asociamos el socket a un puerto	
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	listen(socket_servidor, SOMAXCONN);
	// Escuchamos las conexiones entrantes
	listen(socket_servidor, 10);
	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	int socket_cliente;
	socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}
