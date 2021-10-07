#pragma once

#include "FaceGrid.h"
#include "utility.h"

Any buildFCModels();

void addFrame(Any& entities_a);
void addCubeAt(Any& entities_a, Point3 position, int count);
std::vector<FaceGrid> initFaceGrids();

void decorateFace(Any& entities_a, FaceGrid& f);
void buildFC(Any& entities_a);
void buildFCLights(Any& entities_a);

Any buildFCEntities();