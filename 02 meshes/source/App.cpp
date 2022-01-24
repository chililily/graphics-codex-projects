/** \file App.cpp */
#include "App.h"

// Tells C++ to invoke command-line main() function even on OS X and Win32.
G3D_START_AT_MAIN();

int main(int argc, const char* argv[]) {
    initGLG3D(G3DSpecification());

    GApp::Settings settings(argc, argv);

    // Change the window and other startup parameters by modifying the
    // settings class.  For example:
    settings.window.caption             = "Meshes";

    // Set enable to catch more OpenGL errors
    // settings.window.debugContext     = true;
   
    settings.window.fullScreen          = false;
    if (settings.window.fullScreen) {
        settings.window.width           = 1920;
        settings.window.height          = 1080;
    } else {
        settings.window.height          = int(OSWindow::primaryDisplayWindowSize().y * 1.5f); 
        // Constrain ultra widescreen aspect ratios
        settings.window.width           = min(settings.window.height * 1920 / 1080, int(OSWindow::primaryDisplayWindowSize().x * 1.5f));

        // Make even
        settings.window.width  -= settings.window.width & 1;
        settings.window.height -= settings.window.height & 1;
    }
    settings.window.resizable           = ! settings.window.fullScreen;
    settings.window.framed              = ! settings.window.fullScreen;
    settings.window.defaultIconFilename = "icon.png";

    // Set to true for a significant performance boost if your app can't
    // render at the display frequency, or if you *want* to render faster
    // than the display.
    settings.window.asynchronous        = true;

    // Render slightly larger than the screen so that screen-space refraction, bloom,
    // screen-space AO, and screen-space reflection to look good at screen edges. Set to zero for
    // maximum performance and free memory. Increase the second argument to improve AO without affecting
    // color. The third argument is the resolution. Set to 0.5f to render at half-res and upscale,
    // 2.0f to supersample.
    settings.hdrFramebuffer.setGuardBandsAndSampleRate(64, 0, 1.0f);

    settings.renderer.deferredShading = true;
    settings.renderer.orderIndependentTransparency = true;

    settings.dataDir                       = FileSystem::currentDirectory();

    settings.screenCapture.outputDirectory = FilePath::concat(FileSystem::currentDirectory(), "../journal");
    if (! FileSystem::exists(settings.screenCapture.outputDirectory)) {
        settings.screenCapture.outputDirectory = "";
    }
    settings.screenCapture.includeAppRevision = false;
    settings.screenCapture.includeG3DRevision = false;
    settings.screenCapture.filenamePrefix = "_";

    return App(settings).run();
}


App::App(const GApp::Settings& settings) : GApp(settings) {
}

shared_ptr<Model> App::createCylinderModel(const float r, const float h) {
    const shared_ptr<ArticulatedModel>& model = ArticulatedModel::createEmpty("cylinderModel");

    ArticulatedModel::Part* part = model->addPart("root");
    ArticulatedModel::Geometry* geometry = model->addGeometry("geom");
    ArticulatedModel::Mesh* mesh = model->addMesh("mesh", part, geometry);

    // Assign a material
    mesh->material = UniversalMaterial::create(
        PARSE_ANY(
            UniversalMaterial::Specification{
                lambertian = Color3(0.8, 0.05, 0.1);
                glossy = Color4(Color3(0.1), 0.5);
            }));

    const int   diskFaces = 20;
    const float radius = r * units::meters();
    const float height = h * units::meters();

    Array<CPUVertexArray::Vertex>& vertexArray = geometry->cpuVertexArray.vertex;
    Array<int>& indexArray = mesh->cpuIndexArray;

    const CFrame& frame = (Matrix4::translation(0.0f, height / 2, 0.0f)).approxCoordinateFrame();

    for (int i = 0; i < 2; ++i) {
        float vHeight = i * height;
        // Circular face center
        CPUVertexArray::Vertex& v = vertexArray.next();
        v.position = frame.pointToWorldSpace(Point3(0.0f, vHeight, 0.0f) * radius);
        v.normal = Vector3::nan();
        v.tangent = Vector4::nan();
        const int center = i * (diskFaces + 1);

        for (int p = 0; p < diskFaces; ++p) {
            const float phi = 2.0f * pif() * p / float(diskFaces);

            CPUVertexArray::Vertex& v = vertexArray.next();
            v.position = frame.pointToWorldSpace(Point3(-cos(phi), vHeight, -sin(phi)) * radius);

            // Set to NaN to trigger automatic vertex normal and tangent computation
            v.normal = Vector3::nan();
            v.tangent = Vector4::nan();

            int A;
            int B;
            // Create the circular face triangles
            if (p < diskFaces - 1) {
                A = 1 + center + p;
                B = A + 1;
            }
            else {
                A = center + diskFaces;
                B = center + 1;
            }

            if (i == 0) {
                indexArray.append(A, B, center);
            }
            else {
                indexArray.append(B, A, center);
            }
        }

    }
    // Create cylinder wall faces
        // A-----C
        // |   / |
        // | /   |
        // B-----D
    for (int i = 1; i < diskFaces - 1; ++i) {
        const int A = (i + 0);
        const int B = (i + 0) + (diskFaces + 2);
        const int C = (i + 1);
        const int D = (i + 1) + (diskFaces + 2);
        indexArray.append(
            A, B, C,
            B, D, C);
    }
    const int A = diskFaces;
    const int B = 2 * diskFaces + 1;
    const int C = 1;
    const int D = diskFaces + 2;
    indexArray.append(
        A, B, C,
        B, D, C);

    // Tell the ArticulatedModel to generate bounding boxes, GPU vertex arrays,
    // normals and tangents automatically. We already ensured correct
    // topology, so avoid the vertex merging optimization.
    ArticulatedModel::CleanGeometrySettings geometrySettings;
    geometrySettings.allowVertexMerging = false;
    model->cleanGeometry(geometrySettings);

    return model;
}

void App::addCylinderToScene() {
    // Replace any existing cylinder model. Models don't 
    // have to be added to the model table to use them 
    // with a VisibleEntity.
    const shared_ptr<Model>& cylinderModel = createCylinderModel(0.5, 1.0);
    if (scene()->modelTable().containsKey(cylinderModel->name())) {
        scene()->removeModel(cylinderModel->name());
    }
    scene()->insert(cylinderModel);

    // Replace any existing cylinder entity that has the wrong type
    shared_ptr<Entity> cylinder = scene()->entity("cylinder");
    if (notNull(cylinder) && isNull(dynamic_pointer_cast<VisibleEntity>(cylinder))) {
        logPrintf("The scene contained an Entity named 'cylinder' that was not a VisibleEntity\n");
        scene()->remove(cylinder);
        cylinder.reset();
    }

    if (isNull(cylinder)) {
        // There is no cylinder entity in this scene, so make one.
        //
        // We could either explicitly instantiate a VisibleEntity or simply
        // allow the Scene parser to construct one. The second approach
        // has more consise syntax for this case, since we are using all constant
        // values in the specification.
        cylinder = scene()->createEntity("cylinder",
            PARSE_ANY(
                VisibleEntity{
                    model = "cylinderModel";
                };
        ));
    }
    else {
        // Change the model on the existing cylinder entity
        dynamic_pointer_cast<VisibleEntity>(cylinder)->setModel(cylinderModel);
    }

    cylinder->setFrame(CFrame::fromXYZYPRDegrees(0.0f, 0.0f, 0.0f, 0.0f, 0.0f));
}


// Called before the application loop begins.  Load data here and
// not in the constructor so that common exceptions will be
// automatically caught.
void App::onInit() {
    GApp::onInit();

    setFrameDuration(1.0f / 60.0f);
    showRenderingStats      = false;

    loadScene("Ground");
    addCylinderToScene();

    // Make the GUI after the scene is loaded because loading/rendering/simulation initialize
    // some variables that advanced GUIs may wish to reference with pointers.
    makeGUI();
}


void App::makeGUI() {
    debugWindow->setVisible(true);
    developerWindow->videoRecordDialog->setEnabled(true);
    GuiPane* infoPane = debugPane->addPane("Info", GuiTheme::ORNATE_PANE_STYLE);
    
    // Example of how to add debugging controls
    infoPane->addLabel("You can add GUI controls");
    infoPane->addLabel("in App::onInit().");
    infoPane->addButton("Exit", [this]() { m_endProgram = true; });
    infoPane->pack();

    GuiPane* rendererPane = debugPane->addPane("DefaultRenderer", GuiTheme::ORNATE_PANE_STYLE);

    // showInTextureBrowser("G3D::GBuffer/CS_NORMAL");

    GuiCheckBox* deferredBox = rendererPane->addCheckBox("Deferred Shading",
        Pointer<bool>([&]() {
                const shared_ptr<DefaultRenderer>& r = dynamic_pointer_cast<DefaultRenderer>(m_renderer);
                return r && r->deferredShading();
            },
            [&](bool b) {
                const shared_ptr<DefaultRenderer>& r = dynamic_pointer_cast<DefaultRenderer>(m_renderer);
                if (r) { r->setDeferredShading(b); }
            }));
    rendererPane->addCheckBox("Order-Independent Transparency",
        Pointer<bool>([&]() {
                const shared_ptr<DefaultRenderer>& r = dynamic_pointer_cast<DefaultRenderer>(m_renderer);
                return r && r->orderIndependentTransparency();
            },
            [&](bool b) {
                const shared_ptr<DefaultRenderer>& r = dynamic_pointer_cast<DefaultRenderer>(m_renderer);
                if (r) { r->setOrderIndependentTransparency(b); }
            }));

    GuiPane* giPane = rendererPane->addPane("Ray Tracing", GuiTheme::SIMPLE_PANE_STYLE);
 

    giPane->addCheckBox("Diffuse",
		Pointer<bool>([&]() {
			const shared_ptr<DefaultRenderer>& r = dynamic_pointer_cast<DefaultRenderer>(m_renderer);
			return r && r->enableDiffuseGI();
			},
			[&](bool b) {
				const shared_ptr<DefaultRenderer>& r = dynamic_pointer_cast<DefaultRenderer>(m_renderer);
				if (r) { r->setEnableDiffuseGI(b); }
			}));
    giPane->addCheckBox("Glossy",
        Pointer<bool>([&]() {
            const shared_ptr<DefaultRenderer>& r = dynamic_pointer_cast<DefaultRenderer>(m_renderer);
            return r && r->enableGlossyGI();
            },
            [&](bool b) {
                const shared_ptr<DefaultRenderer>& r = dynamic_pointer_cast<DefaultRenderer>(m_renderer);
                if (r) { r->setEnableGlossyGI(b); }
            }));
    giPane->addCheckBox("Show Probes",
        Pointer<bool>([&]() {
            const shared_ptr<DefaultRenderer>& r = dynamic_pointer_cast<DefaultRenderer>(m_renderer);
            if (notNull(r)) {
                bool allEnabled = r->m_ddgiVolumeArray.size() > 0;
                for (int i = 0; i < r->m_ddgiVolumeArray.size(); ++i) {
                    allEnabled = allEnabled && r->m_showProbeLocations[i];
                }
                return allEnabled;
            }
            return false;
            },
            [&](bool b) {
                const shared_ptr<DefaultRenderer>& r = dynamic_pointer_cast<DefaultRenderer>(m_renderer);
                if (notNull(r)) {
                    for (int i = 0; i < r->m_ddgiVolumeArray.size(); ++i) {
                        r->m_showProbeLocations[i] = b;
                    }
                }
            }), GuiTheme::TOOL_CHECK_BOX_STYLE);
            giPane->pack();

    giPane->moveRightOf(deferredBox);
    giPane->moveBy(100, 0);

    rendererPane->moveRightOf(infoPane);
    rendererPane->moveBy(10, 0);

    // More examples of debugging GUI controls:
    // debugPane->addCheckBox("Use explicit checking", &explicitCheck);
    // debugPane->addTextBox("Name", &myName);
    // debugPane->addNumberBox("height", &height, "m", GuiTheme::LINEAR_SLIDER, 1.0f, 2.5f);
    // button = debugPane->addButton("Run Simulator");
    // debugPane->addButton("Generate Heightfield", [this](){ generateHeightfield(); });
    // debugPane->addButton("Generate Heightfield", [this](){ makeHeightfield(imageName, scale, "model/heightfield.off"); });

    debugWindow->pack();
    debugWindow->setRect(Rect2D::xywh(0, 0, (float)window()->width(), debugWindow->rect().height()));
}