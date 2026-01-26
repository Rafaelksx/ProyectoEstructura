#ifndef CAPANEGOCIO_HPP
#define CAPANEGOCIO_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <regex>
#include <algorithm>
#include <curl/curl.h>

class CapaNegocio {
private:
    std::map<std::string, std::set<std::string>> grafoWeb;
    std::string dominioRaiz; // Ejemplo: "uneg.edu.ve"

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    // Convierte URL a formato estándar
    std::string normalizar(std::string url, std::string actual) {
        if (url.empty() || url.find("mailto:") == 0 || url.find("javascript:") == 0 || url[0] == '#') return "";
        
        std::string resultado = url;
        if (url.find("http") != 0) {
            // Es una ruta relativa
            std::string base = actual.substr(0, actual.find("/", 8));
            if (url[0] == '/') resultado = base + url;
            else resultado = base + "/" + url;
        }
        
        // Limpiar barras finales y anclas
        if (resultado.back() == '/') resultado.pop_back();
        size_t pos = resultado.find("#");
        if (pos != std::string::npos) resultado = resultado.substr(0, pos);
        
        return resultado;
    }

    std::string descargar(std::string url) {
        CURL* curl = curl_easy_init();
        std::string html;
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }
        return html;
    }

public:
    void procesarSitio(std::string semilla, int profMax) {
        grafoWeb.clear();
        
        // Extraer el filtro (ejemplo: de https://www.uneg.edu.ve saca uneg.edu.ve)
        dominioRaiz = semilla;
        if (dominioRaiz.find("www.") != std::string::npos) 
            dominioRaiz = dominioRaiz.substr(dominioRaiz.find("www.") + 4);
        else if (dominioRaiz.find("://") != std::string::npos)
            dominioRaiz = dominioRaiz.substr(dominioRaiz.find("://") + 3);
        
        if (dominioRaiz.find("/") != std::string::npos)
            dominioRaiz = dominioRaiz.substr(0, dominioRaiz.find("/"));

        std::queue<std::pair<std::string, int>> q;
        q.push({semilla, 1});
        std::set<std::string> visitados;

        while(!q.empty()) {
            auto [actual, nivel] = q.front(); q.pop();

            if (nivel > profMax || visitados.count(actual)) continue;
            visitados.insert(actual);

            std::string html = descargar(actual);
            std::regex tag("<a\\s+[^>]*href=\"([^\"]*)\"", std::regex_constants::icase);
            auto start = std::sregex_iterator(html.begin(), html.end(), tag);
            auto end = std::sregex_iterator();

            for (std::sregex_iterator i = start; i != end; ++i) {
                std::string link = normalizar((*i)[1].str(), actual);
                
                // FILTRO CLAVE: Si contiene uneg.edu.ve, entra al grafo y a la cola
                if (!link.empty() && link.find(dominioRaiz) != std::string::npos) {
                    grafoWeb[actual].insert(link);
                    if (nivel < profMax) q.push({link, nivel + 1});
                }
            }
        }
    }


    std::vector<std::string> buscarRuta(std::string inicio, std::string palabra) {
    // Usamos una cola de vectores para guardar el camino completo
    std::queue<std::vector<std::string>> q;
    q.push({inicio});
    
    std::set<std::string> visitados;
    visitados.insert(inicio);

    while (!q.empty()) {
        std::vector<std::string> camino = q.front();
        q.pop();
        std::string actual = camino.back();

        // Si la URL actual contiene la palabra clave, ¡encontramos el camino!
        if (actual.find(palabra) != std::string::npos && actual != inicio) {
            return camino;
        }

        // Explorar los vecinos (enlaces encontrados en esta página)
        if (grafoWeb.count(actual)) {
            for (const auto& vecino : grafoWeb.at(actual)) {
                if (visitados.find(vecino) == visitados.end()) {
                    visitados.insert(vecino);
                    std::vector<std::string> nuevoCamino = camino;
                    nuevoCamino.push_back(vecino);
                    q.push(nuevoCamino);
                }
            }
        }
    }
    return {}; // No se encontró camino
}
    std::map<std::string, std::set<std::string>> getGrafo() { return grafoWeb; }
    std::vector<std::string> buscarCamino(std::string ini, std::string pal); // (Se mantiene igual)


};

#endif