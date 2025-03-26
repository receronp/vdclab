
#include <glm/gtc/matrix_transform.hpp>

#include <uniformGrid.h>
#include <rectilinearGrid.h>
#include <structuredGrid.h>
#include <unstructuredTriangleGrid.h>

#include <PGUPV.h>
#include <GUI3.h>

#include "gridTools.h"

using namespace PGUPV;
using namespace glm;

class MyRender : public Renderer {
public:
	MyRender() {};
	void setup(void) override;
	void render(void) override;
	void reshape(unsigned int w, unsigned int h) override;

private:
	std::shared_ptr<GLMatrices> mats;
	std::shared_ptr<Program> uniformColorProgram, colorMapProgram, hedgeProgram;
	void buildGUI();
	void buildMeshes(std::function<glm::vec2(const glm::vec2 &)> f);
	void prepareMeshForRender(const vdc::Grid<glm::vec2, glm::vec2> &);
	void prepareMeshForRender(const vdc::Grid<glm::vec2, float> &);
	void refreshView();

	std::shared_ptr<Model> func, legend, hedgehog, streamlines;				// Mallas (OpenGL) a dibujar

	std::shared_ptr<Texture1D> colormapTexture;

	std::shared_ptr<TextureText> minLabelTex, maxLabelTex, midLabelTex;

	std::unique_ptr<vdc::UniformGrid<glm::vec2, glm::vec2>> uniformGrid;
	std::shared_ptr<vdc::UniformGrid<glm::vec2, float>> divergence;
	std::shared_ptr<vdc::UniformGrid<glm::vec2, float>> vorticity;

	struct FieldFunction {
		std::string equation;
		std::function<glm::vec2(const glm::vec2 &)> f;
	};
	std::vector<FieldFunction> vectorFields{
	  { "x, y", [](const glm::vec2 &p) { return p; } },
	  { "x^2, y^2", [](const glm::vec2 &p) { return glm::vec2(p.x*p.x, p.y*p.y); } },
	  { "y^2, x^2", [](const glm::vec2 &p) { return glm::vec2(p.y*p.y, p.x*p.x); } },
	  { "1, x+y", [](const glm::vec2 &p) { return glm::vec2(1, p.x + p.y); } },
	  { "x*y, y^2-x^2", [](const glm::vec2 &p) { return glm::vec2(p.x*p.y, p.y*p.y - p.x*p.x); }},
	  { "cos(x^2+y), x-y^2+1", [](const glm::vec2 &p) { return glm::vec2(cos(p.x * p.x + p.y), p.x - p.y * p.y + 1); } },
	  { "1, sin(x^2+y^2)", [](const glm::vec2 &p) { return glm::vec2(1, sin(p.x*p.x + p.y*p.y)); } },
	  { "cos(y^2), sin(x^3)", [](const glm::vec2 &p) { return glm::vec2(cos(p.y*p.y), sin(p.x*p.x*p.x)); } },
	  { "sin^2(y)+y, cos(x+y^2)", [](const glm::vec2 &p) { auto s = sin(p.x);  return glm::vec2(s*s + p.y, cos(p.x + p.y * p.y)); } },
	  { "sin(cos(3 x)+3 y), cos(3 x+sin(3 y))", [](const glm::vec2 &p) { return glm::vec2(sin(cos(3 * p.x) + 3 * p.y), cos(3 * p.x + sin(3 * p.y))); } },
	  { "sin(3 x+cos(3 y)), cos(3 x+sin(3 y))", [](const glm::vec2 &p) { return glm::vec2(sin(3 * p.x + cos(3 * p.y)), cos(3 * p.x + sin(3 * p.y))); } },
	  { "sin(sin(3 x)+cos(3 y)), 3 cos(3 x+cos(3 y))", [](const glm::vec2 &p) { return glm::vec2(sin(sin(3 * p.x) + cos(3 * p.y)), 3 * cos(3 * p.x + cos(3 * p.y))); } }
	};

	// Número de muestras en la malla
	const int horizontalVtcs = 31;
	const int verticalVtcs = 31;

	// Dominio de la malla
	const float minX = -3.0f;
	const float maxX = 3.0f;
	const float minY = -3.0f;
	const float maxY = 3.0f;

	Axes axes;
	std::shared_ptr<CheckBoxWidget> showVertices;
	std::shared_ptr<CheckBoxWidget> showEdges;
	std::shared_ptr<CheckBoxWidget> showStreamlines;
	std::shared_ptr<ListBoxWidget<>> showFunc;
	std::shared_ptr<CheckBoxWidget> showAxes;
	std::shared_ptr<Label> minLabel, maxLabel;

	GLint primitive_color_loc;

	float minValue, maxValue, minDivergence, maxDivergence, minVorticity, maxVorticity;
};


void MyRender::buildMeshes(std::function<glm::vec2(const glm::vec2 &)> f) {
	vdc::sampleVectorField(*uniformGrid, f);
	// Tamaño de los glifos: 0.8 veces el ancho de una celda
	float glyphLength = (maxX - minX) * 0.8f / (horizontalVtcs - 1);
	hedgehog = vdc::computeHedgeHog(*uniformGrid, glyphLength);
	divergence = vdc::computeDivergence(*uniformGrid);
	vorticity = vdc::computeVorticity(*uniformGrid);
	minVorticity = minDivergence = minValue = FLT_MAX;
	maxVorticity = maxDivergence = maxValue = FLT_MIN;
	for (size_t i = 0; i < uniformGrid->numSamples(); i++) {
		auto l = glm::length(uniformGrid->getSampleValue(i));
		minValue = std::min(minValue, l);
		maxValue = std::max(maxValue, l);
		auto d = divergence->getSampleValue(i);
		minDivergence = std::min(minDivergence, d);
		maxDivergence = std::max(maxDivergence, d);
		auto v = vorticity->getSampleValue(i);
		minVorticity = std::min(minVorticity, v);
		maxVorticity = std::max(maxVorticity, v);
	}
	streamlines = std::make_shared<Model>();
	for (size_t i = 0; i < uniformGrid->numSamples(); i += 3) {
		auto samplePos = uniformGrid->getSamplePosition(i);
		auto streamline = vdc::computeStreamline(*uniformGrid, samplePos, 0.05f, 3.0f, 5.0f, glm::vec4());
		streamlines->addMesh(streamline);
	}
	
}

void MyRender::refreshView() {
	TextureTextBuilder tb;
	switch (showFunc->getSelected()) {
	case 0:
		prepareMeshForRender(*uniformGrid);
		minLabel->setText("Valor mínimo = " + std::to_string(minValue));
		maxLabel->setText("Valor máximo = " + std::to_string(maxValue));
		minLabelTex = tb.text(std::to_string(minValue)).build();
		maxLabelTex = tb.text(std::to_string(maxValue)).build();
		midLabelTex = tb.text(std::to_string((maxValue+minValue)/2)).build();
		break;
	case 1:
		prepareMeshForRender(*divergence);
		minLabel->setText("Divergencia mínima = " + std::to_string(minDivergence));
		maxLabel->setText("Divergencia máxima = " + std::to_string(maxDivergence));
		minLabelTex = tb.text(std::to_string(minDivergence)).build();
		maxLabelTex = tb.text(std::to_string(maxDivergence)).build();
		midLabelTex = tb.text(std::to_string((maxDivergence + minDivergence) / 2)).build();
		break;
	case 2:
		prepareMeshForRender(*vorticity);
		minLabel->setText("Vorticidad mínima = " + std::to_string(minVorticity));
		maxLabel->setText("Vorticidad máxima = " + std::to_string(maxVorticity));
		minLabelTex = tb.text(std::to_string(minVorticity)).build();
		maxLabelTex = tb.text(std::to_string(maxVorticity)).build();
		midLabelTex = tb.text(std::to_string((maxVorticity + minVorticity) / 2)).build();
		break;
	default:
		func = std::make_shared<Model>();
		minLabel->setText("");
		maxLabel->setText("");
		minLabelTex.reset();
		maxLabelTex.reset();
		midLabelTex.reset();
	}
}

/**
Transforma la malla seleccionada en una malla OpenGL para dibujar.
*/
void MyRender::prepareMeshForRender(const vdc::Grid<glm::vec2, glm::vec2> &g) {
	// Preparamos los vértices y a la vez calculamos el rango de valores de las muestras
	std::vector<glm::vec2> vertices;
	float min = FLT_MAX, max = FLT_MIN;
	for (size_t i = 0; i < g.numSamples(); i++) {
		vertices.push_back(g.getSamplePosition(i));
		auto v = glm::length(g.getSampleValue(i));
		max = std::max(max, v);
		min = std::min(min, v);
	}

	// Calculamos la coordenada de textura de cada vértice
	std::vector<float> normalizedValue;
	for (size_t i = 0; i < g.numSamples(); i++) {
		normalizedValue.push_back((glm::length(g.getSampleValue(i)) - min) / (max - min));
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
}



/**
Transforma la malla seleccionada en una malla OpenGL para dibujar.
*/
void MyRender::prepareMeshForRender(const vdc::Grid<glm::vec2, float> &g) {
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
	if (max - min < 1e-5) {
		max = (max + min) / 2.0f;
		min = max;
		for (size_t i = 0; i < g.numSamples(); i++) {
			normalizedValue.push_back(min);
		}
	}
	else {
		for (size_t i = 0; i < g.numSamples(); i++) {
			normalizedValue.push_back((g.getSampleValue(i) - min) / (max - min));
		}
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
}


void MyRender::setup() {
	glClearColor(.9f, .9f, .9f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	uniformColorProgram = std::make_shared<Program>();
	uniformColorProgram->addAttributeLocation(Mesh::VERTICES, "position");
	mats = GLMatrices::build();
	uniformColorProgram->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
	uniformColorProgram->loadFiles("../recursos/shaders/constantshadinguniform");
	uniformColorProgram->compile();
	// Obtenemos la localización del uniform que determinará el color de la
	// primitiva
	primitive_color_loc = uniformColorProgram->getUniformLocation("primitive_color");

	colorMapProgram = std::make_shared<Program>();
	colorMapProgram->addAttributeLocation(Mesh::VERTICES, "position");
	colorMapProgram->addAttributeLocation(Mesh::COLORS, "color");
	colorMapProgram->addAttributeLocation(Mesh::TEX_COORD0, "texcoord");
	colorMapProgram->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
	colorMapProgram->loadFiles("../08vectores/vdcmapa");
	colorMapProgram->compile();
	colorMapProgram->use();
	glUniform1i(colorMapProgram->getUniformLocation("colorMapTextureUnit"), 0);


	hedgeProgram = std::make_shared<Program>();
	hedgeProgram->addAttributeLocation(Mesh::VERTICES, "position");
	hedgeProgram->addAttributeLocation(Mesh::COLORS, "vertcolor");
	hedgeProgram->connectUniformBlock(mats, UBO_GL_MATRICES_BINDING_INDEX);
	hedgeProgram->loadFiles("../08vectores/constantshadingA");
	hedgeProgram->compile();
	hedgeProgram->use();


	glActiveTexture(GL_TEXTURE0);
	colormapTexture = std::make_shared<Texture1D>(GL_TEXTURE_1D, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE);
	colormapTexture->loadImage("../recursos/imagenes/scale.png");
	colormapTexture->bind(GL_TEXTURE0);

	buildGUI();

	// Malla uniforme con los datos originales
	uniformGrid = std::unique_ptr<vdc::UniformGrid<glm::vec2, glm::vec2>>(new vdc::UniformGrid<glm::vec2, glm::vec2>(glm::vec2(minX, minY), glm::vec2(maxX, maxY), { horizontalVtcs, verticalVtcs }));
	buildMeshes(vectorFields[0].f);
	refreshView();


	prepareMeshForRender(*uniformGrid);
	glPointSize(5.0f);
	glEnable(GL_POLYGON_OFFSET_FILL);


	// La leyenda
	auto lm = std::make_shared<Mesh>();
	std::vector<glm::vec3> vl{
		glm::vec3(3.1f, -2.0f, 0.0f),
		glm::vec3(3.4f, -2.0f, 0.0f),
		glm::vec3(3.4f,  2.0f, 0.0f),
		glm::vec3(3.1f,  2.0f, 0.0f)
	};
	std::vector<float> tcl{
		0.0f, 0.0f, 1.0f, 1.0f
	};
	lm->addVertices(vl);
	lm->addTexCoord(0, tcl);
	lm->addDrawCommand(new DrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(vl.size())));

	legend = std::make_shared<Model>();
	legend->addMesh(lm);

	// GUI: Por defecto, los widgets son invisibles. Para mostrar la interfaz se
	// puede pulsar Ctrl+H, o hacerlos visibles:
	App::getInstance().getWindow().showGUI(true);
	setCameraHandler(std::make_shared<XYPanZoomCamera>(10.0f, glm::vec2{ 0.8f, 0.0f }));
}

void MyRender::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mats->setMatrix(GLMatrices::VIEW_MATRIX, getCamera().getViewMatrix());
	mats->setMatrix(GLMatrices::PROJ_MATRIX, getCamera().getProjMatrix());

	colorMapProgram->use();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPolygonOffset(0.5f, 1.0f);
	func->render();
	if (minLabelTex) {
		texture2DBlit(glm::vec3(3.7f, -2.0, 0.0f), 0.1f, minLabelTex);
		texture2DBlit(glm::vec3(3.7f, 2.0, 0.0f), 0.1f, maxLabelTex);
		texture2DBlit(glm::vec3(3.7f, 0.0, 0.0f), 0.1f, midLabelTex);

		legend->render();
	}

	hedgeProgram->use();
	hedgehog->render();
	if (showStreamlines->get()) {
		ConstantIllumProgram::use();
		streamlines->render();
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


// GUI: Aquí construímos el interfaz
void MyRender::buildGUI() {
	// Un Panel representa un conjunto de widgets agrupados en una ventana, con un título
	auto panel = addPanel("Parámetros de la visualización");

	// Podemos darle un tamaño y una posición, aunque no es necesario
	panel->setPosition(5, 5);
	panel->setSize(288, 285);

	std::vector<std::string> funcNames;
	for (auto &p : vectorFields)
		funcNames.push_back(p.equation);

	auto fields = std::make_shared<ListBoxWidget<>>("Campo vectorial", funcNames);
	fields->getValue().addListener(
		[this](const int & i) {
		buildMeshes(vectorFields[i].f);
		refreshView();
	}
	);
	panel->addWidget(fields);

	auto backgroundcolor = std::make_shared<RGBAColorWidget>(
		"Color fondo",
		vec4(0.9, 0.9, 0.9, 1.0));
	backgroundcolor->getValue().addListener(
		[](const glm::vec4 &v) {
		glClearColor(v.r, v.g, v.b, v.a);
	});
	panel->addWidget(backgroundcolor);

	showVertices = std::make_shared<CheckBoxWidget>("Mostrar vértices", false);
	panel->addWidget(showVertices);

	showEdges = std::make_shared<CheckBoxWidget>("Mostrar aristas", false);
	panel->addWidget(showEdges);

	std::vector<std::string> options{ "Función", "Divergencia", "Vorticidad", "Nada" };
	showFunc = std::make_shared<ListBoxWidget<>>("Mostrar...", options);
	showFunc->getValue().addListener(
		[this](const int &id) {
		refreshView();
	}
	);
	panel->addWidget(showFunc);

	showStreamlines	= std::make_shared<CheckBoxWidget>("Mostrar líneas de corriente", false);
	panel->addWidget(showStreamlines);

	showAxes = std::make_shared<CheckBoxWidget>("Mostrar ejes", false);
	panel->addWidget(showAxes);

	minLabel = std::make_shared<Label>("Valor mínimo = " + std::to_string(minValue));
	panel->addWidget(minLabel);
	maxLabel = std::make_shared<Label>("Valor máximo = " + std::to_string(maxValue));
	panel->addWidget(maxLabel);
}

int main(int argc, char *argv[]) {
	App &myApp = App::getInstance();
	myApp.setInitWindowSize(1240, 1000);
	myApp.initApp(argc, argv, PGUPV::DOUBLE_BUFFER | PGUPV::DEPTH_BUFFER);
	myApp.getWindow().setRenderer(std::make_shared<MyRender>());
	return myApp.run();
}
