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
    shared_ptr<Model> createCylinderModel(const float r, const float h);
    void addCylinderToScene();

public:
    
    App(const GApp::Settings& settings = GApp::Settings());

    virtual void onInit() override;
};
