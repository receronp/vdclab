
#include <glm/gtc/matrix_transform.hpp>

#include <uniformGrid.h>
#include <rectilinearGrid.h>
#include <structuredGrid.h>
#include <unstructuredTriangleGrid.h>

#include <PGUPV.h>
#include <GUI3.h>

#include "colormap.h"

using namespace PGUPV;
using namespace glm;

class MyRender : public Renderer {
public:
	MyRender() :
		uniformColorProgram(std::make_shared<Program>()),
		colorMapProgram(std::make_shared<Program>())
	{};
	void setup(void) override;
	void render(void) override;
	void reshape(unsigned int w, unsigned int h) override;
	void update(unsigned int ms) override;

private:
	std::shared_ptr<GLMatrices> mats;
	std::shared_ptr<Program> uniformColorProgram, colorMapProgram;
	void buildGUI();
	void buildMeshes();
	void prepareMeshForRender(const vdc::Grid<glm::vec2, float>&);
	void prepareTextures();

	std::shared_ptr<Model> func, legend;				// Mallas (OpenGL) a dibujar
	std::vector<std::pair<std::string, std::shared_ptr<Texture1D>>> textures;
	std::shared_ptr<TextureText> minLabelTex, maxLabelTex;

	std::unique_ptr<vdc::UniformGrid<glm::vec2, float>> uniformGrid;

	Axes axes;
	std::shared_ptr<CheckBoxWidget> showVertices;
	std::shared_ptr<CheckBoxWidget> showEdges;
	std::shared_ptr<CheckBoxWidget> showFunc;
	std::shared_ptr<CheckBoxWidget> showAxes;
	std::shared_ptr<Label> minLabel, maxLabel;

	GLint primitive_color_loc;

	float minValue, maxValue;
};

/**
Aquí se definen las mallas.
*/
void MyRender::buildMeshes() {
	constexpr int horizontalVtcs = 51;
	constexpr int verticalVtcs = 51;

	// Uniforme
	uniformGrid = std::unique_ptr<vdc::UniformGrid<glm::vec2, float>>(new vdc::UniformGrid<glm::vec2, float>(glm::vec2(-2.5f, -2.5f), glm::vec2(2.5f, 2.5f), { horizontalVtcs, verticalVtcs }));
	minValue = FLT_MAX;
	maxValue = FLT_MIN;
	for (size_t i = 0; i < uniformGrid->numSamples(); i++) {
		auto v = uniformGrid->getSamplePosition(i);
		auto f = sin(v.x * v.x) + cos(v.y * v.y);
		minValue = std::min(minValue, f);
		maxValue = std::max(maxValue, f);
		uniformGrid->setSampleValue(i, f);
	}

}

/**
Transforma la malla seleccionada en una malla OpenGL para dibujar.
*/
void MyRender::prepareMeshForRender(const vdc::Grid<glm::vec2, float>& g) {
	// Preparamos los vértices y a la vez calculamos el rango de valores de las muestras
	std::vector<glm::vec2> vertices;
	float min = FLT_MAX, max = FLT_MIN;
	for (size_t i = 0; i < g.numSamples(); i++) {
		vertices.push_back(g.getSamplePosition(i));
		auto v = g.getSampleValue(i);
		max = std::max(max, v);
		min = std::min(min, v);
	}

	// Calculamos la coordenada de textura de cada vértice
	std::vector<float> normalizedValue;
	for (size_t i = 0; i < g.numSamples(); i++) {
		normalizedValue.push_back((g.getSampleValue(i) - min) / (max - min));
	}

	// Preparamos los índices
	std::vector<unsigned int> indices;
	size_t cellVtxIndices[4];
	for (size_t i = 0; i < g.numCells(); i++) {
		auto nv = g.getCellSamples(i, cellVtxIndices);
		indices.push_back(static_cast<GLuint>(cellVtxIndices[0]));
		indices.push_back(static_cast<GLuint>(cellVtxIndices[1]));
		indices.push_back(static_cast<GLuint>(cellVtxIndices[2]));

		if (nv == 4) { // Si la malla está hecha de quads, dibujar el segundo triángulo
			indices.push_back(static_cast<GLuint>(cellVtxIndices[0]));
			indices.push_back(static_cast<GLuint>(cellVtxIndices[2]));
			indices.push_back(static_cast<GLuint>(cellVtxIndices[3]));
		}
	}

	auto m = std::make_shared<Mesh>();
	m->addVertices(vertices);
	m->addIndices(indices);
	m->addTexCoord(0, normalizedValue);
	m->addDrawCommand(new DrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0));

	func = std::make_shared<Model>();
	func->addMesh(m);

	// La leyenda
	auto lm = std::make_shared<Mesh>();
	std::vector<glm::vec3> vl{
	  glm::vec3(2.8f, -2.0f, 0.0f),
	  glm::vec3(3.1f, -2.0f, 0.0f),
	  glm::vec3(3.1f,  2.0f, 0.0f),
	  glm::vec3(2.8f,  2.0f, 0.0f)
	};
	std::vector<float> tcl{
	  0.0f, 0.0f, 1.0f, 1.0f
	};
	lm->addVertices(vl);
	lm->addTexCoord(0, tcl);
	lm->addDrawCommand(new DrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(vl.size())));

	legend = std::make_shared<Model>();
	legend->addMesh(lm);
}


void MyRender::setup() {
	glClearColor(.1f, .1f, .1f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	uniformColorProgram->addAttributeLocation(Mesh::VERTICES, "position");
	mats = GLMatrices::build();
	uniformColorProgram->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
	uniformColorProgram->loadFiles("../recursos/shaders/constantshadinguniform");
	uniformColorProgram->compile();
	// Obtenemos la localización del uniform que determinará el color de la
	// primitiva
	primitive_color_loc = uniformColorProgram->getUniformLocation("primitive_color");


	colorMapProgram->addAttributeLocation(Mesh::VERTICES, "position");
	colorMapProgram->addAttributeLocation(Mesh::COLORS, "color");
	colorMapProgram->addAttributeLocation(Mesh::TEX_COORD0, "texcoord");
	colorMapProgram->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
	colorMapProgram->loadFiles("../06mapacolor/vdcmapa");
	colorMapProgram->compile();
	colorMapProgram->use();

	glUniform1i(colorMapProgram->getUniformLocation("colorMapTextureUnit"), 0);

	buildMeshes();

	// Creamos las texturas
	glActiveTexture(GL_TEXTURE0);
	prepareTextures();
	textures[0].second->bind(GL_TEXTURE0);

	prepareMeshForRender(*uniformGrid);
	glPointSize(5.0f);
	glEnable(GL_POLYGON_OFFSET_FILL);

	TextureTextBuilder tb;
	minLabelTex = tb.text(std::to_string(minValue)).build();
	maxLabelTex = tb.text(std::to_string(maxValue)).build();

	buildGUI();
	// GUI: Por defecto, los widgets son invisibles. Para mostrar la interfaz se
	// puede pulsar Ctrl+H, o hacerlos visibles:
	App::getInstance().getWindow().showGUI(true);
	setCameraHandler(std::make_shared<XYPanZoomCamera>(7.0f, glm::vec2{ 0.8f, 0.0f }));
}

void MyRender::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix());
	mats->setMatrix(GLMatrices::PROJ_MATRIX, getCamera().getProjMatrix());

	if (showFunc->get()) {
		colorMapProgram->use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPolygonOffset(0.5f, 1.0f);
		func->render();
		legend->render();
		if (minLabelTex) {
			texture2DBlit(glm::vec3(3.3f, -2.0, 0.0f), 0.1f, minLabelTex);
			texture2DBlit(glm::vec3(3.3f, 2.0, 0.0f), 0.1f, maxLabelTex);
		}
	}

	uniformColorProgram->use();


	if (showEdges->get()) {
		glUniform4f(primitive_color_loc, 0.1f, 0.1f, 0.1f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonOffset(0.1f, 0.1f);
		func->render();
	}

	if (showVertices->get()) {
		glUniform4f(primitive_color_loc, 1.f, 0.f, 0.f, 1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glPolygonOffset(-0.2f, 0.0f);
		func->render();
	}

	if (showAxes->get()) {
		glLineWidth(4.0f);
		ConstantIllumProgram::use();
		axes.render();
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

void MyRender::prepareTextures() {
	std::shared_ptr<Texture1D> map;
	map = std::make_shared<Texture1D>(GL_TEXTURE_1D, GL_NEAREST, GL_NEAREST);
	auto cols = createRandomColorMap();
	map->loadImageFromMemory(&cols[0], static_cast<unsigned int>(cols.size()), GL_RGBA, GL_FLOAT, GL_RGBA);
	textures.push_back({ "Aleatorio", map });

	cols = createGrayscaleColorMap();
	if (!cols.empty()) {
		map = std::make_shared<Texture1D>(GL_TEXTURE_1D, GL_NEAREST, GL_NEAREST);
		map->loadImageFromMemory(&cols[0], static_cast<unsigned int>(cols.size()), GL_RGBA, GL_FLOAT, GL_RGBA);
		textures.push_back({ "Escala de grises", map });
	}

	cols = createRainbowColorMap();
	if (!cols.empty()) {
		map = std::make_shared<Texture1D>(GL_TEXTURE_1D, GL_NEAREST, GL_NEAREST);
		map->loadImageFromMemory(&cols[0], static_cast<unsigned int>(cols.size()), GL_RGBA, GL_FLOAT, GL_RGBA);
		textures.push_back({ "Arco iris", map });
	}

	cols = createTwoColorMap(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	if (!cols.empty()) {
		map = std::make_shared<Texture1D>(GL_TEXTURE_1D, GL_NEAREST, GL_NEAREST);
		map->loadImageFromMemory(&cols[0], static_cast<unsigned int>(cols.size()), GL_RGBA, GL_FLOAT, GL_RGBA);
		textures.push_back({ "Dos colores", map });
	}

	cols = createHeatColorMap();
	if (!cols.empty()) {
		map = std::make_shared<Texture1D>(GL_TEXTURE_1D, GL_NEAREST, GL_NEAREST);
		map->loadImageFromMemory(&cols[0], static_cast<unsigned int>(cols.size()), GL_RGBA, GL_FLOAT, GL_RGBA);
		textures.push_back({ "Mapa de calor", map });
	}

	cols = createDivergingColorMap(glm::vec4(0, 0, 1, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 0, 0, 1));
	if (!cols.empty()) {
		map = std::make_shared<Texture1D>(GL_TEXTURE_1D, GL_NEAREST, GL_NEAREST);
		map->loadImageFromMemory(&cols[0], static_cast<unsigned int>(cols.size()), GL_RGBA, GL_FLOAT, GL_RGBA);
		textures.push_back({ "Divergente azul-rojo", map });
	}

	cols = createDivergingColorMap(glm::vec4(0, 1, 0, 1), glm::vec4(1, 1, 1, 1), glm::vec4(1, 0, 0, 1));
	if (!cols.empty()) {
		map = std::make_shared<Texture1D>(GL_TEXTURE_1D, GL_NEAREST, GL_NEAREST);
		map->loadImageFromMemory(&cols[0], static_cast<unsigned int>(cols.size()), GL_RGBA, GL_FLOAT, GL_RGBA);
		textures.push_back({ "Divergente verde-rojo", map });
	}
	cols = createZebraColorMap(glm::vec4(0.0), glm::vec4(1.0));
	if (!cols.empty()) {
		map = std::make_shared<Texture1D>(GL_TEXTURE_1D, GL_NEAREST, GL_NEAREST);
		map->loadImageFromMemory(&cols[0], static_cast<unsigned int>(cols.size()), GL_RGBA, GL_FLOAT, GL_RGBA);
		textures.push_back({ "Cebra", map });
	}
}

// GUI: Aquí construímos el interfaz
void MyRender::buildGUI() {
	// Un Panel representa un conjunto de widgets agrupados en una ventana, con un título
	auto panel = addPanel("Parámetros de la visualización");

	// Podemos darle un tamaño y una posición, aunque no es necesario
	panel->setPosition(5, 5);
	panel->setSize(288, 285);

	std::vector<std::string> names;
	for (auto& p : textures)
		names.push_back(p.first);

	auto maps = std::make_shared<ListBoxWidget<>>("Tipo de mapa", names);
	maps->getValue().addListener(
		[this](const int& i) {
			textures[i].second->bind();
		}
	);
	panel->addWidget(maps);

	auto backgroundcolor = std::make_shared<RGBAColorWidget>(
		"Color fondo",
		vec4(0.1, 0.1, 0.1, 1.0));
	backgroundcolor->getValue().addListener(
		[](const glm::vec4& v) {
			glClearColor(v.r, v.g, v.b, v.a);
		});
	panel->addWidget(backgroundcolor);

	showVertices = std::make_shared<CheckBoxWidget>("Mostrar vértices", true);
	panel->addWidget(showVertices);

	showEdges = std::make_shared<CheckBoxWidget>("Mostrar aristas", true);
	panel->addWidget(showEdges);

	showFunc = std::make_shared<CheckBoxWidget>("Mostrar función", true);
	panel->addWidget(showFunc);

	showAxes = std::make_shared<CheckBoxWidget>("Mostrar ejes", false);
	panel->addWidget(showAxes);

	minLabel = std::make_shared<Label>("Valor mínimo = " + std::to_string(minValue));
	panel->addWidget(minLabel);
	maxLabel = std::make_shared<Label>("Valor máximo = " + std::to_string(maxValue));
	panel->addWidget(maxLabel);

}

int main(int argc, char* argv[]) {
	App& myApp = App::getInstance();
	myApp.setInitWindowSize(1240, 1000);
	myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER);
	myApp.getWindow().setRenderer(std::make_shared<MyRender>());
	return myApp.run();
}
