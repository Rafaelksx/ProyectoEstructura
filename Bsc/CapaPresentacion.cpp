#include <iostream>
#include <string>
#include <vector>

class CapaPresentacion {
public:
    void mostrarBienvenida() {
        std::cout << "======================================" << std::endl;
        std::cout << "   ANALIZADOR DE ESTRUCTURA WEB       " << std::endl;
        std::cout << "======================================" << std::endl;
    }

    void menuPrincipal(std::string &url, int &opcion, int &profundidad, std::string &palabra) {
        std::cout << "\n1. Analizar sitio web (URL)" << std::endl;
        std::cout << "2. Buscar camino a palabra clave" << std::endl;
        std::cout << "3. Ver metricas estructurales" << std::endl;
        std::cout << "4. Salir" << std::endl;
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        if (opcion == 1) {
            std::cout << "Ingrese la URL (ej: http://uneg.edu.ve): ";
            std::cin >> url;
            std::cout << "Defina nivel maximo de profundidad: "; 
            std::cin >> profundidad;
        } else if (opcion == 2) {
            std::cout << "Ingrese la palabra clave a buscar en las URL: "; 
            std::cin >> palabra;
        }
    }

    void mostrarResultadoAnalisis(bool exito) {
        if (exito) std::cout << "[OK] Estructura guardada en memoria y archivo." << std::endl; 
        else std::cout << "[ERROR] No se pudo procesar el sitio." << std::endl;
    }
};