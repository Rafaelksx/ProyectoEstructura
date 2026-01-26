#ifndef CAPADATOS_HPP
#define CAPADATOS_HPP

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <queue>

class CapaDatos {
private:
    void escribirRama(std::ofstream& f, const std::map<std::string, std::set<std::string>>& g, 
                      std::string act, int n, int max, std::set<std::string>& v) {
        if (n >= max) return;
        f << "<li><a href='" << act << "'>" << act << "</a> <span class='badge'>Nivel " << n+1 << "</span>";
        if (g.count(act) && v.find(act) == v.end()) {
            v.insert(act);
            f << "<ul>";
            for (auto const& h : g.at(act)) escribirRama(f, g, h, n + 1, max, v);
            f << "</ul>";
        }
        f << "</li>";
    }

public:
    void generarReportes(std::string raiz, const std::map<std::string, std::set<std::string>>& grafo, int prof) {
        // TXT
        std::ofstream txt("resultado.txt");
        txt << "REPORTE: " << raiz << "\n\n";
        for (auto const& [p, hijos] : grafo) {
            txt << p << "\n";
            for (auto const& h : hijos) txt << "  |-- " << h << "\n";
        }
        txt.close();

        // HTML
        std::ofstream html("reporte.html");
        html << "<html><head><style>body{font-family:sans-serif;padding:20px;} ul{border-left:1px solid #ccc;} .badge{font-size:10px;color:#666;}</style></head><body>"
             << "<h1>Arbol: " << raiz << "</h1><ul>";
        std::set<std::string> vis;
        escribirRama(html, grafo, raiz, 0, prof, vis);
        html << "</ul></body></html>";
        html.close();
    }
};

#endif