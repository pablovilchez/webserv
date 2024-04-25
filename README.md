# 🕸 WebServ 🤖

> This repository contains a "webserv" project that is part of the École 42 curriculum.

Webserv project involves creating a web server that handles HTTP requests. In order to understand the project, it's important to have a good understanding of the HTTP protocol and the main concepts of a web server.

## 📚 Guide part

### HTTP Protocol

HTTP stands for Hypertext Transfer Protocol. It is an application-layer protocol that allows communication between clients and servers over the internet. It is the foundation of data communication for the World Wide Web.

The HTTP protocol works on a client-server model, where the client sends a request to the server and the server responds with the requested data. The request and response are made up of different components, including:

- **Request Method**: Specifies the type of request being made (e.g., GET, POST, PUT, DELETE).
- **URL**: The address of the resource being requested.
- **Headers**: Additional information about the request or response (e.g., content type, cookies).
- **Body**: Optional data sent with the request or response.

### Web Server

A web server is a software application that serves web pages to clients upon request. It listens for incoming HTTP requests, processes them, and sends back the requested resources. The main components of a web server include:

- **Listener**: Listens for incoming requests on a specific port.
- **Router**: Routes the incoming requests to the appropriate handler based on the URL.
- **Handler**: Processes the request and generates the appropriate response.
- **Response**: Contains the requested resource and additional information (e.g., headers, status code).

The web server handles multiple client connections simultaneously using various techniques such as multi-threading or asynchronous I/O.

In the context of the Project WebServ, you will be implementing a web server that can handle HTTP requests and serve static files.

### Project Requirements

The WebServ project has the following requirements:

- **HTTP Server**: Implement a web server that can handle HTTP requests.
- **Configuration File**: Read server configuration from a file.
- **Virtual Hosts**: Support multiple virtual hosts with different configurations.
- **Methods**: Implement support for GET, POST, and HEAD methods.
- **Status Codes**: Return appropriate status codes in the response.
- **Error Handling**: Handle errors gracefully and return appropriate error messages.
- **Logging**: Log server events and requests to a log file.
- **Security**: Implement basic security features to protect against common attacks.
- **Performance**: Optimize the server for performance and scalability.
- **Documentation**: Provide clear documentation on how to build, run, and use the server.
- **Bonus**: Implement Python and PHP CGI, and cookie support.

## 🏇 Getting Started

To get started with the WebServ project, follow these steps:

1. **Clone the Repository**: Clone the WebServ repository to your local machine using the following command:

```bash
git clone https://github.com/BredyByte/webserv webserv
```

2. **Build the Project**: Build the project by running the following commands:

```bash
cd webserv/webserver && make
```

3. **Run the Server**: Run the server using the following command:

```bash
./webserv ./config_file
```

4. **Test the Server cURL**: Test the server by sending HTTP requests to it using a web browser or a tool like cURL. File var/curl/CurlCommands.txt contains some curl commands to test the server

5. **Test the Server with sige**: Test the server with sige, a simple HTTP load tester. Install it and execute the following command:

```bash
siege -b -t 50s -c 25 http://127.0.0.1:8080/
```

## 📝 Resources

- [HTTP Protocol](https://developer.mozilla.org/en-US/docs/Web/HTTP) - Mozilla Developer Network, helpful resource for learning about the HTTP protocol. <b>Highly recommended.</b>

- [Beej's Guide to Network Programming Using Internet Sockets](https://beej.us/guide/bgnet/html/) - A comprehensive guide to network programming using sockets. <b>Highly recommended.</b>

- [Web Server](https://en.wikipedia.org/wiki/Web_server) - Wikipedia, overview of web servers and how they work.

- [Nginx Documentation](https://nginx.org/en/docs/) - Official documentation for the Nginx web server, a popular open-source web server.

- [cURL Documentation](https://curl.se/docs/) - Official documentation for cURL, a command-line tool for transferring data with URLs.

## 🙏 Appreciation Corner

This remarkable project was accomplished and refined to its finest form through the assistance of my comrades and collaborators, the ingenious 💡[dbredykh](https://github.com/BredyByte/), the diligent 💪[gkrusta](https://github.com/gkrusta).


