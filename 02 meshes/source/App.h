/**
  \file App.h

  The G3D 10.00 default starter app is configured for OpenGL 4.1 and
  relatively recent GPUs.
 */
#pragma once
#include <G3D/G3D.h>

/** \brief Application framework. */
class App : public GApp {
protected:

    /** Called from onInit */
    void makeGUI();
    
    void saveOFF(const shared_ptr<ArticulatedModel>& model, const String& filename, const String& modelName);

    shared_ptr<ArticulatedModel> createCylinderModel(const float r, const float h);
    void addCylinderToScene(const shared_ptr<Model>& cylinderModel);
    // Generates an OFF file for a convex-polygon mesh for a cylinder of the given radius and height, about the y-axis and centered at the origin
    shared_ptr<Model> makeCylinder(const float r, const float h);

public:
    
    App(const GApp::Settings& settings = GApp::Settings());

    virtual void onInit() override;
};
