#include <iostream>
#include <curl/curl.h>
#include <json/json.h>


// Función callback para escribir los datos recibidos en la respuesta HTTP
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output)
{
    output->append((char *)contents, size * nmemb);
    return size * nmemb;
}

int main(int argc, char **argv)
{
    CURL *curl;
    CURLcode res;
    std::string response;

    if (argc != 2)
    {
        std::cerr << "Error en el número de argumentos" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string num = argv[1];
	std::string url = "https://pokeapi.co/api/v2/pokemon/" + num;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Error al realizar la solicitud: " << curl_easy_strerror(res) << std::endl;
        }
        else 
        {
            Json::Value root;
            Json::Reader reader;
            bool parsingSuccessful = reader.parse(response, root);
            if (!parsingSuccessful)
            {
                //std::cerr << "Error al analizar el JSON: " << reader.getFormattedErrorMessages() << std::endl;
                return 1;
            }

            std::string name = root["name"].asString();
            int height = root["height"].asInt();
            int weight = root["weight"].asInt();
            std::string type = root["types"][0]["type"]["name"].asString();
            std::cout << "Nombre del Pokémon: " << name << std::endl;
            std::cout << "Altura del Pokémon: " << height << " decímetros" << std::endl;
            std::cout << "Peso del Pokémon: " << weight << " hectogramos" << std::endl;
            std::cout << "Tipo de Pokémon: " << type << std::endl;
        }
        curl_easy_cleanup(curl);
    }

    return 0;
}