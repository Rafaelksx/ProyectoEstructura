#include "CapaPresentacion.hpp"
#include "CapaNegocio.hpp"
#include "CapaDatos.hpp"

int main() {
    CapaPresentacion vista;
    CapaNegocio negocio;
    CapaDatos datos;

    int opcion;
    std::string urlRaiz;

    do {
        vista.mostrarMenu();
        std::cin >> opcion;

        if(opcion == 1) {
            int prof;
            std::cout << "URL origen: "; 
            std::cin >> urlRaiz;
            
            // Si el usuario olvida el protocolo, lo agregamos nosotros
            if (urlRaiz.find("http") == std::string::npos) {
                urlRaiz = "https://" + urlRaiz;
            }
            
            std::cout << "Profundidad: "; std::cin >> prof;
            
            negocio.procesarSitio(urlRaiz, prof);
            datos.guardarAnalisisEstructural("resultado_sitio.txt", negocio.getGrafo(), urlRaiz);
            datos.generarHTML("reporte_visual.html", negocio.getGrafo(), urlRaiz);
            std::cout << "[OK] Analisis completado y guardado en archivo.\n";
        } 
        else if(opcion == 2) {
            std::string palabra;
            std::cout << "Palabra clave en URL: "; std::cin >> palabra;
            auto camino = negocio.buscarCamino(urlRaiz, palabra);
            vista.mostrarCamino(camino);
        }

    } while(opcion != 3);

    return 0;
}