#pragma once

#include "FaceGrid.h"
#include "utility.h"

Any buildPCModels();

void addFrame(Any& entities_a);
void addCubeAt(Any& entities_a, Point3 position, int face_id, int count);
void initFaceGrids(std::vector<FaceGrid>& face_grids, int width);

void decorateFace(Any& entities_a, FaceGrid& face_grid, int face_id);
void buildPC(Any& entities_a);
void buildPCLights(Any& entities_a);

Any buildPCEntities();