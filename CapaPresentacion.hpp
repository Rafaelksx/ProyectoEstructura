#ifndef CAPAPRESENTACION_HPP
#define CAPAPRESENTACION_HPP

#include <iostream>
#include <vector>
#include <string>

class CapaPresentacion {
public:
    void mostrarMenu() {
        std::cout << "\n--- ANALIZADOR ESTRUCTURAL WEB ---\n";
        std::cout << "1. Analizar URL y guardar estructura\n";
        std::cout << "2. Buscar camino a palabra clave\n";
        std::cout << "3. Salir\n";
        std::cout << "Opcion: ";
    }

    void mostrarCamino(const std::vector<std::string>& camino) {
        if(camino.empty()) {
            std::cout << "\n[!] No se encontro un camino con esa palabra clave.\n";
            return;
        }
        std::cout << "\nRuta encontrada:\n";
        for(size_t i = 0; i < camino.size(); ++i) {
            std::cout << i << ". " << camino[i] << "\n";
        }
    }
};

#endif