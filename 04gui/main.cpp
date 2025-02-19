
/*
Este ejemplo añade una interfaz de usuario para controlar la visualización
*/
#include <PGUPV.h>
#include <GUI3.h>

using namespace PGUPV;


class MyRender : public Renderer {
public:
	void setup(void) override;
	void render(void) override;
	void reshape(uint w, uint h) override;

private:
	Axes axes;
	std::shared_ptr<GLMatrices> mats;
	/* La clase Model se encarga de almacenar la información necesaria para
	dibujar un objeto. Los modelos están compuestos de mallas (Mesh)*/
	std::shared_ptr<Model> pyramid;
	/*
	Un programa es un conjunto de shaders que se ejecutan en la GPU e indican 
	cómo dibujar los modelos.
	*/
	std::shared_ptr<Program> program;

	std::shared_ptr<Vec3SliderWidget> lightPosWCS;
	GLint lightPosUniformLoc;
	std::shared_ptr<CheckBoxWidget> showAxes;

	// En este método construiremos los elementos de la interfaz gráfica
	void buildGUI();
};

static std::shared_ptr<Model> buildModel() {
	auto model = std::make_shared<Model>();

	// Construímos un objeto Mesh, y lo almacenamos en un puntero inteligente (un puntero
	// que se libera sólo
	auto mesh = std::make_shared<Mesh>();

	// En primer lugar, definimos los vértices de la pirámide
	std::vector<glm::vec3> vtcs{
		glm::vec3(0.0f, 1.0f, 0.0f), // vértice (la punta de la pirámide)
		glm::vec3(-0.5f, 0.0f, -0.5f), // base
		glm::vec3(-0.5f, 0.0f,  0.5f), // base
		glm::vec3(0.5f, 0.0f,  0.5f), // base
		glm::vec3(0.5f, 0.0f, -0.5f) // base
	};

	// Luego los añadimos a la malla:
	mesh->addVertices(vtcs);

	// Luego indicamos con índices cómo dibujar las caras de la pirámide
	std::vector<unsigned int> inds{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 1,
		1, 4, 3,
		1, 3, 2
	};
	// Añadimos los índices a la malla:
	mesh->addIndices(inds);

	// El último paso es añadir una orden de dibujado, donde se indica qué primitivas
	// utilizar:
	mesh->addDrawCommand(new DrawElements(GL_TRIANGLES, static_cast<GLsizei>(inds.size()), GL_UNSIGNED_INT, 0));

	// Ahora podemos añadir la malla al modelo (un modelo puede tener muchas mallas)
	model->addMesh(mesh);

	return model;
}


void MyRender::setup() {
	// Construímos la pirámide
	pyramid = buildModel();


	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// Construímos el objeto GLMatrices
	mats = GLMatrices::build();

	program = std::make_shared<Program>();
	program->addAttributeLocation(Mesh::VERTICES, "position");
	program->addAttributeLocation(Mesh::COLORS, "color");
	program->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
	program->loadFiles("../recursos/shaders/vdc");
	program->compile();
	lightPosUniformLoc = program->getUniformLocation("lightpos");


	// El manejador de cámara OrbitCameraHandler nos permite movernos con el ratón
	// alrededor del origen
	setCameraHandler(std::make_shared<OrbitCameraHandler>());
	
	buildGUI();
	// GUI: Por defecto, los widgets son invisibles. Para mostrar la interfaz se
	// puede pulsar Ctrl+H, o hacerlos visibles:
	App::getInstance().getWindow().showGUI(true);
}

void MyRender::render() {
	// Borramos el buffer de color y el buffer de profundidad
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// La matriz de la vista contiene la posición y orientación actual de la cámara
	mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix());

	glm::vec4 lp = getCamera().getViewMatrix() * glm::vec4(lightPosWCS->get(), 1.0f);
	glUniform4f(lightPosUniformLoc, lp.x, lp.y, lp.z, lp.w);

	if (showAxes->get()) {
		// Instalamos el programa
		ConstantIllumProgram::use();
		// Dibujamos los ejes con el programa instalado
		axes.render();
	}

	program->use();
	pyramid->render();
}

void MyRender::reshape(uint w, uint h) {
	// Zona de dibujo dentro de la ventana (usaremos toda)
	glViewport(0, 0, w, h);
	// El manejador de cámara define por defecto una cámara perspectiva con la misma razón 
	// de aspecto que la ventana
	mats->setMatrix(GLMatrices::PROJ_MATRIX, getCamera().getProjMatrix());
}

void MyRender::buildGUI() {
	// Un Panel representa un conjunto de widgets agrupados en una ventana, con un título
	auto panel = addPanel("Parámetros de la visualización");

	// Podemos darle un tamaño y una posición, aunque no es necesario
	panel->setPosition(5, 5);
	panel->setSize(283, 130);

	// Ahora añadimos un widget para definir el valor de un uniform de tipo vec4 que 
	// representa un color
	auto funcColor = std::make_shared<RGBAColorWidget>(
		"Color",
		glm::vec4(0.7, 0.3, 0.3, 1.0),
		program,
		"diffuseColor");
	panel->addWidget(funcColor);

	lightPosWCS = std::make_shared<Vec3SliderWidget>(
		"Pos. luz", glm::vec3(10.0f, 10.0f, 0.f), -20.f, 20.f);
	panel->addWidget(lightPosWCS);

	showAxes = std::make_shared<CheckBoxWidget>("Mostrar ejes", false);
	panel->addWidget(showAxes);
}

int main(int argc, char *argv[]) {

	App &myApp = App::getInstance();
	// Tamaño inicial de la ventana, en píxeles
	myApp.setInitWindowSize(800, 600);
	myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER);
	myApp.getWindow().setRenderer(std::make_shared<MyRender>());
	return myApp.run();
}
