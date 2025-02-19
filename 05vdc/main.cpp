
#include <glm/gtc/matrix_transform.hpp>

#include <uniformGrid.h>
#include <rectilinearGrid.h>
#include <structuredGrid.h>
#include <unstructuredTriangleGrid.h>

#include <PGUPV.h>
#include <GUI3.h>

#include "buildModel.h"


using namespace PGUPV;
using namespace glm;

class MyRender : public Renderer {
public:
	MyRender() :
		program(std::make_shared<Program>()),
		ambdiff(std::make_shared<Program>())
	{};
	void setup(void) override;
	void render(void) override;
	void reshape(unsigned int w, unsigned int h) override;
	void update(unsigned int ms) override;

private:
	std::shared_ptr<GLMatrices> mats;
	std::shared_ptr<Program> program, ambdiff;
	void buildGUI();
	void buildMeshes();
	void prepareMeshForRender(int i);

	std::shared_ptr<Model> func, derivatives;				// Mallas (OpenGL) a dibujar

	std::unique_ptr<vdc::UniformGrid<glm::vec2, float>> uniformGrid;
	std::unique_ptr<vdc::RectilinearGrid<glm::vec2, float>> rectilinearGrid;
	std::unique_ptr<vdc::StructuredGrid<glm::vec2, float>> structuredGrid;
	std::unique_ptr<vdc::UnstructuredTriangleGrid<glm::vec2, float>> unstructuredGrid;

	Axes axes;
	std::shared_ptr<RGBAColorWidget> funcColor;
	std::shared_ptr<Vec3SliderWidget> lightPosWCS;
	std::shared_ptr<CheckBoxWidget> showVertices;
	std::shared_ptr<CheckBoxWidget> showEdges;
	std::shared_ptr<CheckBoxWidget> showFunc;
	std::shared_ptr<CheckBoxWidget> showAxes;
	std::shared_ptr<CheckBoxWidget> showDerivatives;
	std::shared_ptr<CheckBoxWidget> squashSurface;

	GLint lightPosUniformLoc, primitive_color_loc;
};


void MyRender::buildMeshes() {
	constexpr int horizontalVtcs = 51;
	constexpr int verticalVtcs = 51;
	glm::vec2 domainMin{ -2.5f, -2.5f }, domainMax{ 2.5f, 2.5f };

	// Uniforme
	uniformGrid = vdc::buildUniformGrid(horizontalVtcs, verticalVtcs, domainMin, domainMax);

	// Rectilínea
	rectilinearGrid = vdc::buildRectilinearGrid(horizontalVtcs, verticalVtcs, domainMin, domainMax);

	// Estructurada
	structuredGrid = vdc::buildStructuredGrid(horizontalVtcs, verticalVtcs, domainMin, domainMax);

	// No estructurada
	unstructuredGrid = vdc::buildUnstructuredGrid(horizontalVtcs, verticalVtcs, domainMin, domainMax);
}

/**
Transforma la malla seleccionada en una malla OpenGL para dibujar.
Si la malla tiene celdas cuadradas, además, calcula la derivada
*/
void MyRender::prepareMeshForRender(int i) {
	switch (i) {
	case 0:
	{
		auto deriv = derivative(*uniformGrid);
		derivatives = vdc::buildVectorModelFromGrid<glm::vec2>(*deriv, 1.f, 0.01f);
		func = build2DHeightMapFromGrid(*uniformGrid);
	}
	break;
	case 1:
	{
		auto deriv = derivative(*rectilinearGrid);
		derivatives = vdc::buildVectorModelFromGrid<glm::vec2>(*deriv, 1.f, 0.01f);
		func = build2DHeightMapFromGrid(*rectilinearGrid);
	}
	break;
	case 2:
	{
		derivatives = std::make_shared<Model>();
		func = build2DHeightMapFromGrid(*structuredGrid);
	}
	break;
	case 3:
	{
		derivatives = std::make_shared<Model>();
		func = build2DHeightMapFromGrid(*unstructuredGrid);
	}
	break;
	}
}


void MyRender::setup() {
	glClearColor(.1f, .1f, .1f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	program->addAttributeLocation(Mesh::VERTICES, "position");
	mats = GLMatrices::build();
	program->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
	program->loadFiles("../recursos/shaders/constantshadinguniform");
	program->compile();
	// Obtenemos la localización del uniform que determinará el color de la
	// primitiva
	primitive_color_loc = program->getUniformLocation("primitive_color");


	ambdiff->addAttributeLocation(Mesh::VERTICES, "position");
	ambdiff->addAttributeLocation(Mesh::COLORS, "color");
	ambdiff->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
	ambdiff->loadFiles("../recursos/shaders/vdc");
	ambdiff->compile();
	lightPosUniformLoc = ambdiff->getUniformLocation("lightpos");


	buildGUI();
	// GUI: Por defecto, los widgets son invisibles. Para mostrar la interfaz se
	// puede pulsar Ctrl+H, o hacerlos visibles:
	App::getInstance().getWindow().showGUI(true);
	setCameraHandler(std::make_shared<OrbitCameraHandler>());

	buildMeshes();
	prepareMeshForRender(0);
	glPointSize(5.0f);
	glEnable(GL_POLYGON_OFFSET_FILL);

}

void MyRender::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix());

	if (showFunc->get()) {
		ambdiff->use();
		auto lpEye = mats->getMatrix(GLMatrices::VIEW_MATRIX) * glm::vec4(lightPosWCS->get(), 1.0f);
		glUniform4fv(lightPosUniformLoc, 1, &lpEye.x);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (squashSurface->get()) {
			mats->pushMatrix(GLMatrices::MODEL_MATRIX);
			mats->scale(GLMatrices::MODEL_MATRIX, 1.0f, 0.0f, 1.0f);
		}
		glPolygonOffset(0.5f, 1.0f);
		func->render();
		if (squashSurface->get())
			mats->popMatrix(GLMatrices::MODEL_MATRIX);
	}

	program->use();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	mats->pushMatrix(GLMatrices::MODEL_MATRIX);
	mats->rotate(GLMatrices::MODEL_MATRIX, glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f));
	if (showDerivatives->get()) {
		glUniform4f(primitive_color_loc, 0.0f, 0.7f, 0.7f, 1.0f);
		derivatives->render();
	}
	mats->popMatrix(GLMatrices::MODEL_MATRIX);


	if (squashSurface->get()) {
		mats->pushMatrix(GLMatrices::MODEL_MATRIX);
		mats->scale(GLMatrices::MODEL_MATRIX, 1.0f, 0.0f, 1.0f);
	}

	if (showEdges->get()) {
		glUniform4f(primitive_color_loc, 0.1f, 0.1f, 0.1f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(0.3f, 0.5f);
		func->render();
	}

	if (showVertices->get()) {
		glUniform4f(primitive_color_loc, 1.f, 0.f, 0.f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		func->render();
	}
	if (squashSurface->get()) {
		mats->popMatrix(GLMatrices::MODEL_MATRIX);
	}

	if (showAxes->get()) {
		glLineWidth(4.0f);
		ConstantIllumProgram::use();
		mats->pushMatrix(GLMatrices::MODEL_MATRIX);
		mats->scale(GLMatrices::MODEL_MATRIX, 1.f, 1.f, -1.f);
		axes.render();
		mats->popMatrix(GLMatrices::MODEL_MATRIX);
		glLineWidth(1.0f);
	}

	CHECK_GL();
}

void MyRender::reshape(unsigned int w, unsigned int h) {
	glViewport(0, 0, w, h);
	if (h == 0)
		h = 1;
	float ar = (float)w / h;
	mats->setMatrix(GLMatrices::PROJ_MATRIX,
		glm::perspective(glm::radians(60.0f), ar, .1f, 100.0f));
}

void MyRender::update(unsigned int ms) {
}

// GUI: Aquí construímos el interfaz
void MyRender::buildGUI() {
	// Un Panel representa un conjunto de widgets agrupados en una ventana, con un título
	auto panel = addPanel("Parámetros de la visualización");

	// Podemos darle un tamaño y una posición, aunque no es necesario
	panel->setPosition(5, 5);
	panel->setSize(445, 350);


	auto meshSelector = std::shared_ptr<ListBoxWidget<>>(new ListBoxWidget<>("Malla", { "Uniforme", "Rectilínea", "Estructurada", "No estructurada" }));
	meshSelector->getValue().addListener(
		[this](const int& m) {
			prepareMeshForRender(m);
		}
	);
	panel->addWidget(meshSelector);

	auto backgroundcolor = std::make_shared<RGBAColorWidget>(
		"Color fondo",
		vec4(0.1, 0.1, 0.1, 1.0));
	backgroundcolor->getValue().addListener(
		[](const glm::vec4& v) {
			glClearColor(v.r, v.g, v.b, v.a);
		});
	panel->addWidget(backgroundcolor);

	funcColor = std::make_shared<RGBAColorWidget>(
		"Color de la malla",
		vec4(0.7, 0.3, 0.3, 1.0),
		ambdiff,
		"diffuseColor");
	panel->addWidget(funcColor);

	lightPosWCS = std::make_shared<Vec3SliderWidget>(
		"Pos. luz", glm::vec3(10.0f, 10.0f, 0.f), -20.f, 20.f);
	panel->addWidget(lightPosWCS);

	showVertices = std::make_shared<CheckBoxWidget>("Mostrar vértices", true);
	panel->addWidget(showVertices);

	showEdges = std::make_shared<CheckBoxWidget>("Mostrar aristas", true);
	panel->addWidget(showEdges);

	showFunc = std::make_shared<CheckBoxWidget>("Mostrar función", true);
	panel->addWidget(showFunc);

	showAxes = std::make_shared<CheckBoxWidget>("Mostrar ejes", false);
	panel->addWidget(showAxes);

	squashSurface = std::make_shared<CheckBoxWidget>("Aplanar malla", false);
	panel->addWidget(squashSurface);

	auto useColorMap = std::make_shared<CheckBoxWidget>("Mapa de colores", false, ambdiff, "useVertexColor");
	panel->addWidget(useColorMap);

	showDerivatives = std::make_shared<CheckBoxWidget>("Mostrar derivada", true);
	panel->addWidget(showDerivatives);
}

int main(int argc, char* argv[]) {
	App& myApp = App::getInstance();
	myApp.setInitWindowSize(1240, 1000);
	myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER);
	myApp.getWindow().setRenderer(std::make_shared<MyRender>());
	return myApp.run();
}
