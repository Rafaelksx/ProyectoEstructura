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
    std::string dominioRaiz;


    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    std::string normalizar(std::string url, std::string actual) {
        if (url.empty() || url.find("mailto:") == 0 || url.find("javascript:") == 0 || url[0] == '#') return "";
        std::string res = url;
        if (url.find("http") != 0) {
            std::string base = actual.substr(0, actual.find("/", 8));
            res = (url[0] == '/') ? base + url : base + "/" + url;
        }
        if (res.back() == '/') res.pop_back();
        return res;
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

    struct Metricas {
        int totalPaginas;
        int totalEnlaces;
        double promedioEnlaces;
        std::string paginaMasConectada;
        int maxEnlaces;
    };


    void procesarSitio(std::string semilla, int profMax) {
        grafoWeb.clear();
        std::set<std::string> visitados; // Registro de páginas YA descargadas
        std::queue<std::pair<std::string, int>> q;

        // Extraer dominio para el filtro
        dominioRaiz = semilla;
        size_t p = dominioRaiz.find("://");
        if (p != std::string::npos) dominioRaiz = dominioRaiz.substr(p + 3);
        if (dominioRaiz.find("www.") == 0) dominioRaiz = dominioRaiz.substr(4);
        if (dominioRaiz.find("/") != std::string::npos) dominioRaiz = dominioRaiz.substr(0, dominioRaiz.find("/"));

        q.push({semilla, 1});

        while(!q.empty()) {
            auto [actual, nivel] = q.front(); q.pop();

            // REGLA DE ORO: Si ya la visitamos, no descargamos su HTML otra vez
            if (nivel > profMax || visitados.count(actual)) continue;
            visitados.insert(actual);

            std::string html = descargar(actual);
            if(html.empty()) continue;

            std::regex tag("<a\\s+[^>]*href=\"([^\"]*)\"", std::regex_constants::icase);
            auto start = std::sregex_iterator(html.begin(), html.end(), tag);
            auto end = std::sregex_iterator();

            for (std::sregex_iterator i = start; i != end; ++i) {
                std::string link = normalizar((*i)[1].str(), actual);
                
                if (!link.empty() && link.find(dominioRaiz) != std::string::npos) {
                    grafoWeb[actual].insert(link); // Guardamos la relación
                    
                    // Solo agregamos a la cola si NO ha sido visitada para evitar redundancia
                    if (visitados.find(link) == visitados.end() && nivel < profMax) {
                        q.push({link, nivel + 1});
                    }
                }
            }
        }
    }

    std::vector<std::string> buscarRuta(std::string inicio, std::string palabra) {
        std::queue<std::vector<std::string>> q;
        q.push({inicio});
        std::set<std::string> visRuta;
        visRuta.insert(inicio);

        while (!q.empty()) {
            std::vector<std::string> camino = q.front(); q.pop();
            std::string actual = camino.back();

            if (actual.find(palabra) != std::string::npos && actual != inicio) return camino;

            if (grafoWeb.count(actual)) {
                for (const auto& v : grafoWeb.at(actual)) {
                    if (visRuta.find(v) == visRuta.end()) {
                        visRuta.insert(v);
                        std::vector<std::string> nuevo = camino;
                        nuevo.push_back(v);
                        q.push(nuevo);
                    }
                }
            }
        }
        return {};
    }


    Metricas calcularMetricas() {
         Metricas m = {0, 0, 0.0, "", 0};
        m.totalPaginas = grafoWeb.size();
        
        for (auto const& [padre, hijos] : grafoWeb) {
            int numHijos = hijos.size();
            m.totalEnlaces += numHijos;
            
            if (numHijos > m.maxEnlaces) {
                m.maxEnlaces = numHijos;
                m.paginaMasConectada = padre;
            }
        }
        
        if (m.totalPaginas > 0) {
            m.promedioEnlaces = (double)m.totalEnlaces / m.totalPaginas;
        }
        
        return m;
    }

    std::map<std::string, std::set<std::string>> getGrafo() { return grafoWeb; }
};

#endif