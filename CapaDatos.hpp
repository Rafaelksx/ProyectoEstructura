#ifndef CAPADATOS_HPP
#define CAPADATOS_HPP

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>

class CapaDatos {
public:
    void guardarAnalisisEstructural(const std::string& nombreArchivo, 
                               const std::map<std::string, std::set<std::string>>& grafo,
                               std::string raiz) {
    std::ofstream archivo(nombreArchivo);
        if (!archivo.is_open()) return;

        archivo << "ESTRUCTURA DEL SITIO (TEXTO PLANO)\n";
        archivo << "Origen: " << raiz << "\n\n";

        std::set<std::string> visitados;
        std::queue<std::pair<std::string, int>> q;
        q.push({raiz, 0});
        visitados.insert(raiz);

        while(!q.empty()) {
            auto [actual, nivel] = q.front();
            q.pop();

            std::string sangria(nivel * 4, ' ');
            archivo << sangria << "|-- " << actual << " (Nivel " << nivel + 1 << ")\n";

            if (grafo.count(actual)) {
                for (const auto& hijo : grafo.at(actual)) {
                    if (visitados.find(hijo) == visitados.end()) {
                        q.push({hijo, nivel + 1});
                        visitados.insert(hijo);
                    }
                }
            }
        }
        archivo.close();
    }

    // Genera el reporte visual HTML con CSS 
    void generarHTML(const std::string& nombre, const std::map<std::string, std::set<std::string>>& grafo, const std::string& raiz) {
        std::ofstream file(nombre);
        file << "<!DOCTYPE html><html><head><meta charset='UTF-8'><style>"
             << "body { font-family: 'Segoe UI', sans-serif; background: #eceff1; color: #37474f; padding: 40px; }"
             << "h1 { color: #263238; border-bottom: 2px solid #607d8b; padding-bottom: 10px; }"
             << "ul { list-style: none; padding-left: 25px; }"
             << "li { margin: 8px 0; position: relative; border-left: 1px dashed #b0bec5; padding-left: 20px; }"
             << "li::before { content: ''; position: absolute; top: 12px; left: 0; width: 15px; height: 1px; background: #b0bec5; }"
             << "a { color: #0277bd; text-decoration: none; font-size: 14px; transition: color 0.3s; }"
             << "a:hover { color: #01579b; text-decoration: underline; }"
             << ".badge { background: #cfd8dc; color: #546e7a; font-size: 10px; padding: 2px 6px; border-radius: 10px; margin-left: 8px; }"
             << "</style></head><body>"
             << "<h1>Árbol Estructural: " << raiz << "</h1><ul>";

        std::set<std::string> visitados;
        escribirRamaHTML(file, grafo, raiz, 0, visitados);

        file << "</ul></body></html>";
        file.close();
    }

private:
    void escribirRamaHTML(std::ofstream& f, const std::map<std::string, std::set<std::string>>& g, std::string act, int n, std::set<std::string>& v) {
        f << "<li><a href='" << act << "' target='_blank'>" << act << "</a><span class='badge'>Nivel " << n+1 << "</span>";
        if (g.count(act) && v.find(act) == v.end()) {
            v.insert(act);
            f << "<ul>";
            for (const auto& h : g.at(act)) {
                escribirRamaHTML(f, g, h, n + 1, v);
            }
            f << "</ul>";
        }
        f << "</li>";
    }
};

#endif