#include "CapaPresentacion.hpp"
#include "CapaNegocio.hpp"
#include "CapaDatos.hpp"

int main() {
    CapaPresentacion vista;
    CapaNegocio negocio;
    CapaDatos datos;

    int opcion;
    std::string urlActual = ""; // Declarada aqui para que sea global al main

    do {
        opcion = vista.mostrarMenu(); // Ahora mostrarMenu devuelve int

        if (opcion == 1) {
            urlActual = vista.pedirURL();
            int prof = vista.pedirProfundidad();
            
            vista.mensajeProcesando();
            negocio.procesarSitio(urlActual, prof);
            datos.generarReportes(urlActual, negocio.getGrafo(), prof);
            vista.mensajeExito();
        } 
        else if (opcion == 2) {
            if (urlActual.empty()) {
                std::cout << "[!] Primero analice un sitio." << std::endl;
            } else {
                std::string palabra = vista.pedirPalabra();
                std::vector<std::string> ruta = negocio.buscarRuta(urlActual, palabra);
                vista.mostrarResultadoRuta(ruta);
            }
        }
    } while (opcion != 3);

    return 0;
}