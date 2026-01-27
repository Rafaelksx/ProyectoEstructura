#ifndef CAPADATOS_HPP
#define CAPADATOS_HPP

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>

class CapaDatos {
private:

// Genera un string unico basado en la fecha y hora: AAAAMMDD_HHMMSS
    std::string obtenerTimestamp() {
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
        return oss.str();
    }

    // Funcion recursiva para escribir el arbol en HTML
    void escribirRama(std::ofstream& f, const std::map<std::string, std::set<std::string>>& g, 
                      std::string act, int n, int max, std::set<std::string>& dibujados) {
        if (n >= max) return;

        f << "<li><a href='" << act << "' target='_blank'>" << act << "</a> <span class='b'>Nivel " << n+1 << "</span>";

        // Si la página tiene hijos y NO ha sido expandida antes en este reporte
        if (g.count(act) && dibujados.find(act) == dibujados.end()) {
            dibujados.insert(act); 
            f << "<ul>";
            for (auto const& h : g.at(act)) {
                escribirRama(f, g, h, n + 1, max, dibujados);
            }
            f << "</ul>";
        } else if (g.count(act)) {
            // Si ya se dibujó antes, ponemos una marca de "ya analizado"
            f << " <small>(Ver arriba)</small>";
        }
        
        f << "</li>";
    }

public:

    // Genera reportes en TXT y HTML
    void generarReportes(std::string raiz, const std::map<std::string, std::set<std::string>>& grafo, int prof, const CapaNegocio::Metricas& m) {
        std::string ts = obtenerTimestamp();
        
        // 1. GENERAR TXT ÚNICO
        std::string nombreTxt = "resultado_" + ts + ".txt";
        std::ofstream txt(nombreTxt);
        txt << "REPORTE ESTRUCTURAL - " << raiz << "\n";
        txt << "Fecha: " << ts << "\n\n";
        for (auto const& [padre, hijos] : grafo) {
            txt << padre << " [" << hijos.size() << " enlaces]\n";
            for (auto const& h : hijos) txt << "  |-- " << h << "\n";
            txt << "\n";
        }
        txt.close();

        // 2. GENERAR HTML ÚNICO
        std::string nombreHtml = "reporte_" + ts + ".html";
        std::ofstream html(nombreHtml);
        html << "<html><head><meta charset='UTF-8'><style>"
             << "body{font-family:sans-serif; background:#f8f9fa; padding:20px;}"
             << "ul{list-style:none; border-left:1px solid #ccc; padding-left:20px;}"
             << "li{margin:8px 0; border-bottom:1px solid #eee;}"
             << "a{text-decoration:none; color:#007bff;}"
             << ".b{font-size:11px; background:#e9ecef; padding:2px 5px;}"
             << "</style></head><body><h1>Mapa Web: " << raiz << "</h1><ul>";
        std::set<std::string> vis;
        escribirRama(html, grafo, raiz, 0, prof, vis);
        html << "</ul></body></html>";
        html.close();

        // 3. ACTUALIZAR EL LOG (HISTORIAL CENTRAL)
        guardarLog(raiz, prof, m);
        
        std::cout << "\n[DATOS] Archivos generados:\n - " << nombreTxt << "\n - " << nombreHtml << std::endl;
    }


    // Guarda un log de la ejecucion con parametros y metricas
    void guardarLog(std::string url, int prof, const CapaNegocio::Metricas& m) {
        std::ofstream log("ejecucion.log", std::ios::app); // std::ios::app para no borrar lo anterior
        if (!log.is_open()) return;

        // Obtener fecha y hora actual
        time_t ahora = time(0);
        char* dt = ctime(&ahora);

        log << "================================================" << std::endl;
        log << "FECHA Y HORA: " << dt;
        log << "PARAMETROS UTILIZADOS:" << std::endl;
        log << " - URL Semilla: " << url << std::endl;
        log << " - Profundidad: " << prof << std::endl;
        log << "METRICAS OBTENIDAS:" << std::endl;
        log << " - Nodos (Paginas): " << m.totalPaginas << std::endl;
        log << " - Aristas (Enlaces): " << m.totalEnlaces << std::endl;
        log << " - Densidad (Promedio): " << m.promedioEnlaces << " enlaces/pag" << std::endl;
        log << " - Nodo Critico: " << m.paginaMasConectada << " (" << m.maxEnlaces << " hijos)" << std::endl;
        log << "================================================\n" << std::endl;

        log.close();
    }
};

#endif