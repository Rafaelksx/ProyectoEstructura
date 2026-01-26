#include "CapaNegocio.h"
#include <iostream>

size_t CapaNegocio::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::vector<std::string> CapaNegocio::analizarEstructura(std::string urlSemilla, int maxProfundidad) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::vector<std::string> enlacesEncontrados;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, urlSemilla.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res == CURLE_OK) {
            // Aquí llamarías a una función auxiliar para extraer 
            // los <a href="..."> del readBuffer que coincidan con el dominio. [cite: 6, 7]
        }
    }
    return enlacesEncontrados;
}