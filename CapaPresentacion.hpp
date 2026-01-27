#ifndef CAPAPRESENTACION_HPP
#define CAPAPRESENTACION_HPP

#include <iostream>
#include <string>
#include <vector>

class CapaPresentacion {
public:
    int mostrarMenu() {
        int op;
        std::cout << "\n====================================" << std::endl;
        std::cout << "   ANALIZADOR ESTRUCTURAL (UNEG)" << std::endl;
        std::cout << "====================================" << std::endl;
        std::cout << "1. Analizar sitio y generar reportes" << std::endl;
        std::cout << "2. Buscar camino a palabra clave" << std::endl;
        std::cout << "3. Salir" << std::endl;
        std::cout << "Opcion: ";
        std::cin >> op;
        return op;
    }

    std::string pedirURL() {
        std::string url;
        std::cout << "URL Semilla (ej: https://uneg.edu.ve): ";
        std::cin >> url;
        if (url.find("http") == std::string::npos) url = "https://" + url;
        return url;
    }

    int pedirProfundidad() {
        int p;
        std::cout << "Nivel de profundidad: ";
        std::cin >> p;
        return p;
    }

    std::string pedirPalabra() {
        std::string p;
        std::cout << "Introduce la palabra clave: ";
        std::cin >> p;
        return p;
    }

    void mostrarMetricas(int totalPaginas, int totalEnlaces, double promedio, std::string masConectada, int maxHijos) {
    std::cout << "\n---------- METRICAS ESTRUCTURALES ----------" << std::endl;
    std::cout << " > Total de paginas analizadas: " << totalPaginas << std::endl;
    std::cout << " > Total de enlaces encontrados: " << totalEnlaces << std::endl;
    std::cout << " > Promedio de enlaces por pagina: " << promedio << std::endl;
    std::cout << " > Pagina mas conectada: " << masConectada << std::endl;
    std::cout << "   (con " << maxHijos << " enlaces directos)" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    }

    void mensajeProcesando() { std::cout << "\n[!] Rastreando estructura..." << std::endl; }
    void mensajeExito() { std::cout << "[OK] Reportes generados." << std::endl; }

    void mostrarResultadoRuta(const std::vector<std::string>& ruta) {
        if (ruta.empty()) std::cout << "\n[!] No se encontro la palabra clave." << std::endl;
        else {
            std::cout << "\n[OK] Camino hallado:" << std::endl;
            for (size_t i = 0; i < ruta.size(); ++i) std::cout << " [" << i << "] " << ruta[i] << std::endl;
        }
    }
};

#endif