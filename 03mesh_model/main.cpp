
/*
Esta aplicación muestra cómo dibujar con PGUPV
*/
#include <PGUPV.h>

using namespace PGUPV;


class MyRender : public Renderer {
public:
	void setup(void) override;
	void render(void) override;
	void reshape(uint w, uint h) override;

private:
	/* La clase Model se encarga de almacenar la información necesaria para
	dibujar un objeto. Los modelos están compuestos de mallas (Mesh)*/
	std::shared_ptr<Model> roof;
	std::shared_ptr<Model> house;
	std::shared_ptr<Model> square;
	std::shared_ptr<Model> circle;

	/*
	Un programa es un conjunto de shaders que se ejecutan en la GPU e indican
	cómo dibujar los modelos.
	*/
	Axes axes;
	std::shared_ptr<GLMatrices> mats;
};

static std::shared_ptr<Model> buildRoof() {
	auto model = std::make_shared<Model>();

	// Construímos un objeto Mesh, y lo almacenamos en un puntero inteligente (un puntero
	// que se libera sólo cuando ya no face falta)
	auto mesh = std::make_shared<Mesh>();

	// En primer lugar, definimos los vértices del tejado
	std::vector<glm::vec3> vtcs{
		glm::vec3(0.0f, 1.0f, 0.0f),   // vértice (la punta del tejado)
		glm::vec3(-0.5f, 0.5f, -0.5f), // base
		glm::vec3(-0.5f, 0.5f, 0.5f),  // base
		glm::vec3(0.5f, 0.5f, 0.5f),   // base
		glm::vec3(0.5f, 0.5f, -0.5f)   // base
	};

	// Luego los añadimos a la malla:
	mesh->addVertices(vtcs);

	// Colores por vértice
	std::vector<glm::vec4> cols{
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // vértice (la punta del tejado)
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), // base
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), // base
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), // base
		glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)  // base
	};
	mesh->addColors(cols);

	// Luego indicamos con índices cómo dibujar las caras del tejado
	std::vector<unsigned int> inds{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 1,
		1, 4, 3,
		1, 3, 2};
	// Añadimos los índices a la malla:
	mesh->addIndices(inds);

	// El último paso es añadir una orden de dibujado, donde se indica qué primitivas
	// utilizar:
	mesh->addDrawCommand(new DrawElements(GL_TRIANGLES, static_cast<GLsizei>(inds.size()), GL_UNSIGNED_INT, 0));

	// Ahora podemos añadir la malla al modelo (un modelo puede tener muchas mallas, todas definidas
	// con respecto al mismo sistema de coordenadas local)
	model->addMesh(mesh);

	return model;
}

static std::shared_ptr<Model> buildHouse() {
	auto model = std::make_shared<Model>();

	// Construímos un objeto Mesh, y lo almacenamos en un puntero inteligente (un puntero
	// que se libera sólo cuando ya no face falta)
	auto mesh = std::make_shared<Mesh>();

	std::vector<glm::vec3> vtcs{
		glm::vec3(-0.4f, 0.0f, -0.4f),	// base
		glm::vec3(-0.4f, 0.0f, 0.4f),	// base
		glm::vec3(0.4f, 0.0f, 0.4f),	// base
		glm::vec3(0.4f, 0.0f, -0.4f),	// base

		glm::vec3(-0.4f, 0.5f, -0.4f), // top
		glm::vec3(-0.4f, 0.5f, 0.4f),  // top
		glm::vec3(0.4f, 0.5f, 0.4f),   // top
		glm::vec3(0.4f, 0.5f, -0.4f)   // top
	};

	mesh->addVertices(vtcs);

	// Colores por vértice
	std::vector<glm::vec4> cols{
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),	// base
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),	// base
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),	// base
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),	// base

		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // top
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), // top
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), // top
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)  // top
	};
	mesh->addColors(cols);

	std::vector<unsigned int> inds{
		0, 1, 2,
		0, 2, 3,
		0, 1, 5,
		0, 5, 4,
		1, 2, 6,
		1, 6, 5,
		2, 3, 7,
		2, 7, 6,
		3, 0, 4,
		3, 4, 7,};
	mesh->addIndices(inds);

	// El último paso es añadir una orden de dibujado, donde se indica qué primitivas
	// utilizar:
	mesh->addDrawCommand(new DrawElements(GL_TRIANGLES, static_cast<GLsizei>(inds.size()), GL_UNSIGNED_INT, 0));

	// Ahora podemos añadir la malla al modelo (un modelo puede tener muchas mallas, todas definidas
	// con respecto al mismo sistema de coordenadas local)
	model->addMesh(mesh);

	return model;
}

static std::shared_ptr<Model> buildSquare() {
	auto model = std::make_shared<Model>();

	// Construímos un objeto Mesh, y lo almacenamos en un puntero inteligente (un puntero
	// que se libera sólo cuando ya no face falta)
	auto mesh = std::make_shared<Mesh>();

	std::vector<glm::vec2> vtcs{
		glm::vec2(-1.0f, -1.0f),
		glm::vec2(-1.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, -1.0f),
	};
	mesh->addVertices(vtcs);

	std::vector<unsigned int> inds{0, 1, 2, 3};
	mesh->addIndices(inds);

	// El último paso es añadir una orden de dibujado, donde se indica qué primitivas
	// utilizar:
	mesh->addDrawCommand(new DrawElements(GL_LINE_LOOP, static_cast<GLsizei>(inds.size()), GL_UNSIGNED_INT, 0));

	// Ahora podemos añadir la malla al modelo (un modelo puede tener muchas mallas, todas definidas
	// con respecto al mismo sistema de coordenadas local)
	model->addMesh(mesh);

	return model;
}

static std::shared_ptr<Model> buildCircle() {
	auto model = std::make_shared<Model>();

	// Construímos un objeto Mesh, y lo almacenamos en un puntero inteligente (un puntero
	// que se libera sólo cuando ya no face falta)
	auto mesh = std::make_shared<Mesh>();

	std::vector<glm::vec2> vtcs;
	std::vector<unsigned int> inds;
	std::vector<glm::vec4> cols(36, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	for (int i = 0; i < 360; i += 10)
	{
		float angle = glm::radians(static_cast<float>(i));
		vtcs.push_back(glm::vec2(cos(angle), sin(angle)));
		inds.push_back(static_cast<unsigned int>(i / 10));
	}
	mesh->addVertices(vtcs);
	mesh->addIndices(inds);
	mesh->addColors(cols);

	// El último paso es añadir una orden de dibujado, donde se indica qué primitivas
	// utilizar:
	mesh->addDrawCommand(new DrawElements(GL_LINE_LOOP, static_cast<GLsizei>(inds.size()), GL_UNSIGNED_INT, 0));

	// Ahora podemos añadir la malla al modelo (un modelo puede tener muchas mallas, todas definidas
	// con respecto al mismo sistema de coordenadas local)
	model->addMesh(mesh);

	return model;
}

void MyRender::setup() {
	// Construímos el tejado
	roof = buildRoof();
	house = buildHouse();
	square = buildSquare();
	circle = buildCircle();

	// A partir de aquí el código se encarga de configurar la aplicación para poder inspeccionar
	// el modelo.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// Construímos el objeto GLMatrices
	mats = GLMatrices::build();

	// El manejador de cámara OrbitCameraHandler nos permite movernos con el ratón
	// alrededor del origen
	setCameraHandler(std::make_shared<OrbitCameraHandler>());
}

void MyRender::render() {
	// Borramos el buffer de color y el buffer de profundidad
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// La matriz de la vista contiene la posición y orientación actual de la cámara
	mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix());
	// Instalamos el programa
	ConstantIllumProgram::use();
	// Dibujamos los ejes con el programa instalado
	axes.render();
	// Dibujamos el objeto
	roof->render();
	house->render();
	square->render();
	circle->render();
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
