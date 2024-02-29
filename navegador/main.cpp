#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int main() {
    // Crear el socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error al crear el socket" << std::endl;
        return -1;
    }

    // Configurar la dirección del servidor
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080); // Puerto HTTP estándar
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Enlazar el socket a la dirección del servidor
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {
        std::cerr << "Error al enlazar el socket" << std::endl;
        return -2;
    }

    // Escuchar por conexiones entrantes
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error al escuchar por conexiones entrantes" << std::endl;
        return -3;
    }

    // Aceptar conexiones entrantes
    sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);
    if (clientSocket == -1) {
        std::cerr << "Error al aceptar la conexión entrante" << std::endl;
        return -4;
    }

    // Leer y procesar la solicitud HTTP
    char buffer[1024];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        std::cerr << "Error al leer la solicitud HTTP" << std::endl;
        close(clientSocket);
        return -5;
    }

    // Imprimir la solicitud HTTP recibida
    std::cout << "Solicitud HTTP recibida:" << std::endl;
    std::cout.write(buffer, bytesRead);

    // Enviar una respuesta HTTP de ejemplo
    const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
	//const char* response = "HTTP/1.1 401 Unauthorized";
    ssize_t bytesSent = send(clientSocket, response, strlen(response), 0);
    if (bytesSent == -1) {
        std::cerr << "Error al enviar la respuesta HTTP" << std::endl;
        close(clientSocket);
        return -6;
    }

    // Cerrar el socket del cliente
    close(clientSocket);

    // Cerrar el socket del servidor
    close(serverSocket);

    return 0;
}
