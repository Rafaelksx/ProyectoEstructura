#include <string>
#include <vector>
#include <curl/curl.h>

class CapaNegocio {
public:
    // Función para obtener el HTML de una URL usando libcurl
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    
    // Procesa el sitio respetando profundidad y dominio [cite: 7, 8]
    std::vector<std::string> analizarEstructura(std::string urlSemilla, int maxProfundidad);

    // Punto B: Buscar camino a palabra clave [cite: 9, 10]
    std::vector<std::string> buscarCamino(std::string inicio, std::string palabraClave);

    // Punto C: Métricas [cite: 11]
    double calcularDensidadEnlaces();
};