#ifndef CAPADATOS_HPP
#define CAPADATOS_HPP

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

class CapaDatos {
private:
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
    void generarReportes(std::string raiz, const std::map<std::string, std::set<std::string>>& grafo, int prof) {
        std::ofstream txt("resultado.txt");
        txt << "REPORTE ESTRUCTURAL (OPTIMIZADO)\nRaiz: " << raiz << "\n\n";
        for (auto const& [padre, hijos] : grafo) {
            txt << padre << " tiene " << hijos.size() << " enlaces directos.\n";
            for (auto const& h : hijos) txt << "  |-- " << h << "\n";
            txt << "\n";
        }
        txt.close();

        std::ofstream html("reporte.html");
        html << "<html><head><style>"
             << "body{font-family:'Segoe UI',sans-serif; background:#f8f9fa; padding:20px;}"
             << "ul{list-style:none; border-left:1px solid #ccc; padding-left:20px;}"
             << "li{margin:8px 0; border-bottom:1px solid #eee; padding-bottom:4px;}"
             << "a{text-decoration:none; color:#007bff;}"
             << ".b{font-size:11px; background:#e9ecef; border-radius:3px; padding:2px 5px;}"
             << "</style></head><body><h1>Mapa Web Jerarquico</h1><ul>";
        
        std::set<std::string> visHTML;
        escribirRama(html, grafo, raiz, 0, prof, visHTML);
        
        html << "</ul></body></html>";
        html.close();
    }
};

#endif