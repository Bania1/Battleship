inicio: compilarServidor compilarCliente fin

compilarServidor: servidor.c funciones.c
	@echo "Compilando el servidor..."
	gcc servidor.c funciones.c -o servidor

compilarCliente: cliente.c
	@echo "Compilando el cliente..."
	gcc cliente.c funciones.c -o cliente

clean: servidor cliente
	@echo "Borrando ejecutables..."
	rm servidor cliente

fin:
	@echo "Juego compilado!"

