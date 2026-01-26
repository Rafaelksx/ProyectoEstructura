#ifndef CAPANEGOCIO_HPP
#define CAPANEGOCIO_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <regex>
#include <iostream>
#include <curl/curl.h>

class CapaNegocio {
private:
    // Estructura en memoria solicitada (Grafo: URL -> Enlaces del mismo dominio) [cite: 6, 10]
    std::map<std::string, std::set<std::string>> grafoWeb;
    std::string dominioBase;

    // Función auxiliar para que libcurl guarde el contenido descargado
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    // Descarga el HTML de una URL [cite: 12]
    std::string descargarHTML(std::string url) {
        CURL* curl = curl_easy_init();
        std::string buffer;
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
            curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert-2025-12-02.pem");
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); 
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            CURLcode res = curl_easy_perform(curl);
            if(res != CURLE_OK) {
                std::cerr << "Error en CURL: " << curl_easy_strerror(res) << std::endl;
            }
            curl_easy_cleanup(curl);
        }
        return buffer;
    }

    // Normaliza URLs para manejar rutas relativas (ej: /contacto -> https://dominio.com/contacto)
    std::string normalizarURL(std::string encontrado, std::string urlActual) {
        if (encontrado.empty()) return "";
        
        // 1. Ignorar anclas internas, scripts y correos
        if (encontrado[0] == '#' || encontrado.find("javascript:") == 0 || encontrado.find("mailto:") == 0) {
            return "";
        }

        // 2. Manejar rutas relativas que empiezan con /
        if (encontrado[0] == '/') {
            return dominioBase + encontrado;
        }

        // 3. Si ya es una URL completa, la dejamos así
        if (encontrado.find("http") == 0) {
            return encontrado;
        }

        // 4. Otros casos (rutas relativas sin / al inicio)
        return dominioBase + "/" + encontrado;
    }

public:
    // Punto A: Análisis de estructura guardando en memoria [cite: 6, 8]
    void procesarSitio(std::string urlSemilla, int maxProfundidad) {
        grafoWeb.clear();
        
        // Extraemos el dominio base sin el "www." para que el filtro sea más amplio
        std::string filtroDominio = urlSemilla;
        size_t startPos = filtroDominio.find("://");
        if (startPos != std::string::npos) {
            filtroDominio = filtroDominio.substr(startPos + 3);
        }
        if (filtroDominio.find("www.") == 0) {
            filtroDominio = filtroDominio.substr(4); // Quitamos "www."
        }
        // Si queda una barra al final, la quitamos
        if (filtroDominio.back() == '/') filtroDominio.pop_back();

        std::queue<std::pair<std::string, int>> q;
        q.push({urlSemilla, 0});
        std::set<std::string> visitados;

        while(!q.empty()) {
            auto [urlActual, nivel] = q.front();
            q.pop();

            if(nivel > maxProfundidad || visitados.count(urlActual)) continue;

            if (urlActual.find("http") != 0) continue;

            visitados.insert(urlActual);
            std::string html = descargarHTML(urlActual);

            if (html.empty()) continue;
            
            std::regex linkRegex("<a\\s+(?:[^>]*?\\s+)?href=\"([^\"]*)\"", std::regex_constants::icase);
            auto words_begin = std::sregex_iterator(html.begin(), html.end(), linkRegex);
            auto words_end = std::sregex_iterator();

            for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
                std::string encontrado = (*i)[1].str();
                std::string urlLimpia = normalizarURL(encontrado, urlActual);

                // Ahora el filtro busca el dominio raíz (uneg.edu.ve)
                // Esto aceptará tanto con www como sin www
                if(!urlLimpia.empty() && urlLimpia.find(filtroDominio) != std::string::npos) {
                    grafoWeb[urlActual].insert(urlLimpia);
                    q.push({urlLimpia, nivel + 1});
                }
            }
        }
    }

    // Punto B: Indicar enlaces necesarios para llegar a la palabra clave (BFS) [cite: 9, 10]
    std::vector<std::string> buscarCamino(std::string inicio, std::string palabra) {
        if (grafoWeb.empty()) return {};

        std::queue<std::vector<std::string>> q;
        q.push({inicio});
        std::set<std::string> visitados;

        while(!q.empty()) {
            std::vector<std::string> camino = q.front();
            q.pop();
            std::string actual = camino.back();

            // Verificamos si la URL actual contiene la palabra clave [cite: 9]
            if(actual.find(palabra) != std::string::npos && actual != inicio) {
                return camino;
            }

            if(!visitados.count(actual)) {
                visitados.insert(actual);
                for(const auto& vecino : grafoWeb[actual]) {
                    std::vector<std::string> nuevoCamino = camino;
                    nuevoCamino.push_back(vecino);
                    q.push(nuevoCamino);
                }
            }
        }
        return {}; // No existe la conexión [cite: 9]
    }

    // Punto C: Métricas estructurales [cite: 11]
    std::map<std::string, double> calcularMetricas() {
        std::map<std::string, double> m;
        m["Total Paginas"] = (double)grafoWeb.size();
        
        double totalEnlaces = 0;
        for(auto const& [url, enlaces] : grafoWeb) {
            totalEnlaces += enlaces.size();
        }
        
        if(m["Total Paginas"] > 0)
            m["Promedio Enlaces por Pagina"] = totalEnlaces / m["Total Paginas"];
            
        return m;
    }

    std::map<std::string, std::set<std::string>> getGrafo() { return grafoWeb; }
};

#endif