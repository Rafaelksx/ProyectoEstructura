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
public:
    struct Metricas {
        int totalPaginas;
        int totalEnlaces;
        double promedioEnlaces;
        std::string paginaMasConectada;
        int maxEnlaces;
    };

    
private:
    std::map<std::string, std::set<std::string>> grafoWeb;
    std::string filtroDominio;

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    // NORMALIZACIÓN AVANZADA: Unifica URLs para evitar duplicados falsos
    /**
     * @brief Normaliza una URL dada basándose en una URL de referencia (real).
     *
     * Esta función procesa la URL de entrada para producir una versión normalizada adecuada para su uso en un gráfico web o un rastreador.
     * La normalización incluye:
     *   - Ignorar las URL vacías, los enlaces mailto, los enlaces javascript y los enlaces que solo contienen fragmentos.
     *   - Resolver las URL relativas utilizando la base de la URL real.
     *   - Convertir la URL a minúsculas.
     *   - Eliminar fragmentos de URL (cualquier cosa después de «#»).
     *   - Convertir «http://» a «https://».
     *   - Eliminar «www.» para tratar las URL con y sin «www» como equivalentes.
     * - Eliminación de la barra inclinada final.
    *
    * @param url La URL que se va a normalizar (puede ser relativa o absoluta).
    * @param actual La URL base con respecto a la cual se van a resolver las URL relativas.
    * @return std::string La URL normalizada o una cadena vacía si la entrada no es un enlace navegable válido.
    */
    std::string normalizarURL(std::string url, std::string actual) {
        if (url.empty() || url.find("mailto:") == 0 || url.find("javascript:") == 0 || url[0] == '#') return "";

        std::string res = url;
        // Manejo de rutas relativas
        if (url.find("http") != 0) {
            std::string base = actual.substr(0, actual.find("/", 8));
            res = (url[0] == '/') ? base + url : base + "/" + url;
        }

        // 1. Convertir a minúsculas
        std::transform(res.begin(), res.end(), res.begin(), ::tolower);
        // 2. Eliminar fragmentos (#)
        size_t hashPos = res.find('#');
        if (hashPos != std::string::npos) res = res.substr(0, hashPos);
        // 3. Unificar protocolo (usar siempre https si es posible para el grafo)
        if (res.find("http://") == 0) res.replace(0, 7, "https://");
        // 4. Quitar 'www.' para que uneg.edu.ve y www.uneg.edu.ve sean lo mismo
        size_t wwwPos = res.find("://www.");
        if (wwwPos != std::string::npos) res.erase(wwwPos + 3, 4);
        // 5. Quitar barra final
        if (res.back() == '/') res.pop_back();

        return res;
    }

    /**
     * @brief Descarga el contenido HTML de una URL especificada.
     *
     * Esta función utiliza la biblioteca cURL para realizar una solicitud HTTP(S) a la URL proporcionada
     * y devuelve el contenido HTML como una cadena de texto. Configura varias opciones de cURL, incluyendo
     * el seguimiento de redirecciones, desactivación de la verificación SSL, un tiempo de espera y un
     * User-Agent personalizado.
     *
     * @param url La URL desde la cual se descargará el contenido HTML.
     * @return std::string El contenido HTML descargado de la URL. Si ocurre un error, se devuelve una cadena vacía.
     */
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
            // USER-AGENT para parecer un navegador real
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AnalizadorUNEG/1.0");
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }
        return html;
    }

public:
    /**
     * @brief Procesa un sitio web a partir de una URL semilla, recorriendo sus enlaces hasta una profundidad máxima.
     *
     * Esta función realiza un rastreo (web crawling) a partir de la URL proporcionada como semilla, explorando los enlaces internos
     * del mismo dominio hasta alcanzar la profundidad máxima especificada. Construye un grafo de enlaces encontrados y evita visitar
     * la misma URL más de una vez. Solo se consideran enlaces que pertenecen al mismo dominio raíz que la semilla.
     *
     * @param semilla URL inicial desde donde comienza el rastreo.
     * @param profMax Profundidad máxima de exploración de enlaces.
     */
    void procesarSitio(std::string semilla, int profMax) {
        grafoWeb.clear();
        std::set<std::string> visitados;
        std::queue<std::pair<std::string, int>> q;

        std::string inicio = normalizarURL(semilla, semilla);
        
        // Extraer dominio raíz para el filtro (ej: uneg.edu.ve)
        filtroDominio = inicio;
        size_t p = filtroDominio.find("://");
        if (p != std::string::npos) filtroDominio = filtroDominio.substr(p + 3);
        if (filtroDominio.find("/") != std::string::npos) filtroDominio = filtroDominio.substr(0, filtroDominio.find("/"));

        q.push({inicio, 1});

        while(!q.empty()) {
            auto [actual, nivel] = q.front(); q.pop();

            if (nivel > profMax || visitados.count(actual)) continue;
            visitados.insert(actual);

            std::string html = descargar(actual);
            if(html.empty()) continue;

            std::regex tag("<a\\s+[^>]*href=\"([^\"]*)\"", std::regex_constants::icase);
            auto start = std::sregex_iterator(html.begin(), html.end(), tag);
            auto end = std::sregex_iterator();

            for (std::sregex_iterator i = start; i != end; ++i) {
                std::string link = normalizarURL((*i)[1].str(), actual);
                
                if (!link.empty() && link.find(filtroDominio) != std::string::npos) {
                    grafoWeb[actual].insert(link);
                    if (visitados.find(link) == visitados.end() && nivel < profMax) {
                        q.push({link, nivel + 1});
                    }
                }
            }
        }
    }
    
    /**
     * @brief Calcula varias métricas sobre el grafo web almacenado.
     *
     * Este método recorre el grafo web y calcula:
     * - El número total de páginas (nodos) en el grafo.
     * - El número total de enlaces (aristas) entre páginas.
     * - El número máximo de enlaces salientes desde una sola página.
     * - La página con más enlaces salientes.
     * - El promedio de enlaces por página.
     *
     * @return Metricas Estructura que contiene las métricas calculadas del grafo web.
     */
    Metricas calcularMetricas() {
        Metricas m = {0, 0, 0.0, "", 0};
        m.totalPaginas = grafoWeb.size();
        for (auto const& [padre, hijos] : grafoWeb) {
            m.totalEnlaces += hijos.size();
            if ((int)hijos.size() > m.maxEnlaces) {
                m.maxEnlaces = hijos.size();
                m.paginaMasConectada = padre;
            }
        }
        if (m.totalPaginas > 0) m.promedioEnlaces = (double)m.totalEnlaces / m.totalPaginas;
        return m;
    }

    /**
     * @brief Busca una ruta en el grafo web desde una URL de inicio hasta una URL que contenga una palabra clave.
     *
     * Esta función realiza una búsqueda en anchura (BFS) en el grafo web representado por `grafoWeb`, 
     * comenzando desde la URL `inicio` (normalizada) y buscando la primera ruta hacia una URL que contenga 
     * la subcadena `palabra` en su nombre (exceptuando la URL de inicio). 
     * Devuelve el camino encontrado como un vector de strings, donde cada elemento es una URL en la ruta.
     *
     * @param inicio URL de inicio desde donde comienza la búsqueda.
     * @param palabra Palabra clave que debe estar contenida en la URL destino.
     * @return std::vector<std::string> Ruta desde la URL de inicio hasta la URL que contiene la palabra clave.
     *         Si no se encuentra ninguna ruta, retorna un vector vacío.
     */
    std::vector<std::string> buscarRuta(std::string inicio, std::string palabra) {
        std::string ini = normalizarURL(inicio, inicio);
        std::queue<std::vector<std::string>> q;
        q.push({ini});
        std::set<std::string> visRuta;
        visRuta.insert(ini);

        while (!q.empty()) {
            std::vector<std::string> camino = q.front(); q.pop();
            std::string actual = camino.back();
            if (actual.find(palabra) != std::string::npos && actual != ini) return camino;
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

    std::map<std::string, std::set<std::string>> getGrafo() { return grafoWeb; }
};
#endif