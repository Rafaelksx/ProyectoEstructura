#ifndef CAPADATOS_HPP
#define CAPADATOS_HPP

#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include "CapaNegocio.hpp"

class CapaDatos {
private:
    /**
     * @brief Obtiene la marca de tiempo actual como una cadena formateada.
     *
     * Esta función recupera la hora local actual y la formatea como una cadena
     * en el formato «YYYYMMDD_HHMMSS», lo que puede ser útil para nombres de archivo,
     * registros u otros fines en los que se requiera una marca de tiempo.
     *
     * @return std::string La marca de tiempo formateada string.
     */
    std::string obtenerTimestamp() {
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
        return oss.str();
    }

    /**
     * @brief Escribe recursivamente una rama de un grafo en formato HTML como una lista anidada.
     *
     * Esta función genera una estructura HTML en forma de lista (<ul><li></li></ul>) que representa
     * la expansión de un grafo dirigido, donde cada nodo es un enlace. Se controla la profundidad
     * máxima de expansión y se evita la expansión repetida de nodos ya visitados.
     *
     * @param f Referencia al flujo de salida de archivo donde se escribe el HTML.
     * @param g Grafo representado como un mapa de nodos a conjuntos de nodos adyacentes.
     * @param act Nodo actual a procesar y escribir.
     * @param n Nivel actual de profundidad en la recursión.
     * @param max Profundidad máxima permitida para la expansión.
     * @param v Conjunto de nodos ya visitados para evitar ciclos o expansiones repetidas.
     */
    void escribirRama(std::ofstream& f, const std::map<std::string, std::set<std::string>>& g, 
                  std::string act, int n, int max, std::set<std::string>& v) {
    
        // Imprimimos el nodo siempre (así el Nivel 3 aparecerá)
        f << "<li><a href='" << act << "' target='_blank'>" << act << "</a> <span class='b'>Nivel " << n+1 << "</span>";

        // Solo intentamos expandir si NO hemos superado el límite de profundidad
        // y si el nodo tiene hijos en el grafo.
        if (n + 1 < max && g.count(act) && v.find(act) == v.end()) {
            v.insert(act);
            f << "<ul>";
            for (auto const& h : g.at(act)) {
                escribirRama(f, g, h, n + 1, max, v);
            }
            f << "</ul>";
        } else if (g.count(act) && !g.at(act).empty()) {
            // Si tiene hijos pero no los mostramos por el nivel, avisamos
            f << " <small>(Límite de profundidad)</small>";
        }
        
        f << "</li>";
    }

public:

    /**
     * @brief Genera reportes estructurales en formato TXT y HTML a partir de un grafo de enlaces.
     *
     * Esta función crea dos archivos de reporte: uno en texto plano (.txt) y otro en HTML (.html),
     * que describen la estructura del grafo proporcionado, comenzando desde la raíz especificada.
     * Además, registra la operación en un log.
     *
     * @param raiz Nodo raíz desde el cual se inicia el reporte.
     * @param grafo Grafo representado como un mapa de nodos y sus conjuntos de hijos (enlaces).
     * @param prof Profundidad máxima a la que se debe explorar el grafo para el reporte HTML.
     * @param m Estructura de métricas calculadas sobre el grafo (por ejemplo, total de páginas).
     */
    void generarReportes(std::string raiz, const std::map<std::string, std::set<std::string>>& grafo, int prof, const CapaNegocio::Metricas& m) {
        std::string ts = obtenerTimestamp();
        
        // TXT ÚNICO
        std::ofstream txt("resultado_" + ts + ".txt");
        txt << "REPORTE ESTRUCTURAL\nRaiz: " << raiz << "\nNodos: " << m.totalPaginas << "\n\n";
        for (auto const& [padre, hijos] : grafo) {
            txt << padre << " (" << hijos.size() << " enlaces)\n";
            for (auto const& h : hijos) txt << "  |-- " << h << "\n";
            txt << "\n";
        }
        txt.close();

        // HTML ÚNICO
        std::ofstream html("reporte_" + ts + ".html");
        html << "<html><head><style>body{font-family:sans-serif; background:#f8f9fa; padding:20px;}"
             << "ul{list-style:none; border-left:1px solid #ccc; padding-left:20px;}"
             << "li{margin:5px 0; border-bottom:1px solid #eee;}"
             << "a{text-decoration:none; color:#007bff;}"
             << ".b{font-size:11px; background:#e9ecef; padding:2px 5px;}</style></head><body>"
             << "<h1>Mapa Web: " << raiz << "</h1><ul>";
        std::set<std::string> vis;
        escribirRama(html, grafo, raiz, 0, prof, vis);
        html << "</ul></body></html>";
        html.close();

        guardarLog(raiz, prof, m, ts);
    }

    /**
     * @brief Guarda una entrada de log en el archivo "ejecucion.log" con información sobre la ejecución.
     *
     * Esta función escribe una línea en el archivo de log que incluye la marca de tiempo, la URL semilla,
     * la profundidad, el número total de páginas y enlaces procesados.
     *
     * @param url URL semilla utilizada en la ejecución.
     * @param prof Profundidad alcanzada durante la ejecución.
     * @param m Estructura de métricas que contiene el total de páginas y enlaces procesados.
     * @param ts Marca de tiempo (timestamp) en formato de cadena.
     */
    void guardarLog(std::string url, int prof, const CapaNegocio::Metricas& m, std::string ts) {
        std::ofstream log("ejecucion.log", std::ios::app);
        log << "[" << ts << "] SEMILLA: " << url << " | PROF: " << prof 
            << " | NODOS: " << m.totalPaginas << " | ENLACES: " << m.totalEnlaces << "\n";
        log.close();
    }
};
#endif