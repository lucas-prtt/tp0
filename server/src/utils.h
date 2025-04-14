#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>
#include <pthread.h>
#define PUERTO "4444"
typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;
void iterator(char* value);
typedef struct{
	int socket;
	void * parametros;
}infoAtencionThread;
typedef struct{
	int ejemplo;
}parametrosAtencionThread;



extern t_log* logger;
void * atenderConThread(void * puntero);
int esperarClientes(int socket_server, void (*atenderCliente)(void*), parametrosAtencionThread * parametros);
void* recibir_buffer(int*, int);

int iniciar_servidor(void);
int esperar_cliente(int);
t_list* recibir_paquete(int);
void recibir_mensaje(int);
int recibir_operacion(int);
int crearSocketServer(char * puerto);
#endif /* UTILS_H_ */


