#include <fstream>
#include <map>
#include <set>

class CapaDatos {
public:
    // Guarda todos los enlaces que pertenecen al mismo dominio [cite: 6, 7]
    bool exportarEstructura(const std::string& nombreArchivo, 
                           const std::map<std::string, std::set<std::string>>& grafo) {
        std::ofstream archivo(nombreArchivo);
        if (!archivo.is_open()) return false;

        archivo << "ESTRUCTURA DEL SITIO WEB\n";
        archivo << "--------------------------\n";
        for (auto const& [origen, destinos] : grafo) {
            archivo << "Origen: " << origen << "\n";
            for (const auto& destino : destinos) {
                archivo << "  -> " << destino << "\n";
            }
        }
        archivo.close();
        return true;
    }
};