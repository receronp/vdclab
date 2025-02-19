
/*
Esta aplicación muestra los ejes coordenados en 3D
*/

// Para utilizar la librería PGUPV tienes que incluir la siguiente cabecera:
#include <PGUPV.h>

/*
Todas las clases de la librería PGUPV están definidas dentro del namespace
PGUPV. Para usar cualquier clase en C++, debes especificar el espacio de nombres
donde se encuentra. Para ello, tienes tres opciones:

1. Calificar con el espacio de nombres cada aparición de la clase en el código:
// Declarando el objeto program
PGUPV::Program program;

2. Declarar que el uso de una clase hace referencia a un namespace dado:

using PGUPV::Program;
// Declarando el objeto program
Program program;

3. Importar todas las clases de un namespace:

using namespace PGUPV;
// Declarando el objeto program
Program program;

En los ejemplos de la asignatura usaremos indistintamente la opción 2 o 3.
*/

using namespace PGUPV;

/*
El primer paso para usar la librería PGUPV es crear una clase que herede de
PGUPV::Renderer
Deberás implementar, como mínimo, los métodos setup, render y reshape.
*/

class MyRender : public Renderer {
public:
	void setup(void) override;
	void render(void) override;
	void reshape(uint w, uint h) override;

	/*
	El primer paso para usar la librería PGUPV es crear una clase que herede de
	PGUPV::Renderer
	Deberás implementar, como mínimo, los métodos setup, render y reshape.
	*/
private:
	/* El objeto Axes contiene unos ejes predefinidos (eje X rojo, eje Y verde y
	 eje Z azul) */
	Axes axes;
	/* Esta variable contiene las matrices modelo, vista y proyección que se
	 utilizarán en el programa */
	std::shared_ptr<GLMatrices> mats;
};

void MyRender::setup() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


	// Construímos el objeto GLMatrices
	mats = GLMatrices::build();

	// El manejador de cámara OrbitCameraHandler nos permite movernos con el ratón
	// alrededor del origen
	setCameraHandler(std::make_shared<OrbitCameraHandler>());
}

void MyRender::render() {
	// Borramos el buffer de color 
	glClear(GL_COLOR_BUFFER_BIT);
	// La matriz de la vista contiene la posición y orientación actual de la cámara
	mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix());
	// Instalamos el programa
	ConstantIllumProgram::use();
	// Dibujamos los ejes con el programa instalado
	axes.render();
}

void MyRender::reshape(uint w, uint h) {
	// Zona de dibujo dentro de la ventana (usaremos toda)
	glViewport(0, 0, w, h);
	// El manejador de cámara define por defecto una cámara perspectiva con la misma razón 
	// de aspecto que la ventana
	mats->setMatrix(GLMatrices::PROJ_MATRIX, getCamera().getProjMatrix());
}

int main(int argc, char *argv[]) {
	
	App &myApp = App::getInstance();
	// Tamaño inicial de la ventana, en píxeles
	myApp.setInitWindowSize(800, 600);
	myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER);
	myApp.getWindow().setRenderer(std::make_shared<MyRender>());
	return myApp.run();
}
