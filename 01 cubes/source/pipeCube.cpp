#include "pipeCube.h"

Any buildPCModels()
{
    Any models_a{ Any::TABLE };

    Any unit_spec{ Any::TABLE };
    unit_spec.setName("ArticulatedModel::Specification");
    unit_spec["filename"] = "model/cube/cube.obj";

    Any unit_preprocess{ Any::ARRAY, "", "{}", ';' };
    TextInput unit_setMaterial{ TextInput::FROM_STRING, "setMaterial(all(), \"material/roughcedar/roughcedar.UniversalMaterial.Any\");" };
    unit_preprocess.append(Any{ unit_setMaterial });
    unit_spec["preprocess"] = unit_preprocess;

    models_a["unitModel"] = unit_spec;

    Any frame_spec{ Any::TABLE };
    frame_spec.setName("ArticulatedModel::Specification");
    frame_spec["filename"] = "model/cube/cube.obj";

    Any frame_preprocess{ Any::ARRAY, "", "{}", ';' };
    TextInput frame_setMaterial{ TextInput::FROM_STRING, "setMaterial(all(), \"material/roughcedar/roughcedar.UniversalMaterial.Any\");" };
    TextInput frame_transform{ TextInput::FROM_STRING, "transformGeometry(all(), Matrix4::scale(8, 1, 1));" };
    frame_preprocess.append(Any{ frame_setMaterial });
    frame_preprocess.append(Any{ frame_transform });
    frame_spec["preprocess"] = frame_preprocess;

    models_a["frameEdgeModel"] = frame_spec;

    return models_a;
}

// Build cube frame (edges)
void addFrame(Any& entities_a)
{
    char cframestr_buffer[128];
    char cframestr[]{ "CFrame::fromXYZYPRDegrees(%f, %f, %f, %i, 0., %i);" };
    double x;
    double y;
    double z;

    Any fc_frameEdge{ Any::TABLE };
    fc_frameEdge.setName("VisibleEntity");
    fc_frameEdge["model"] = "frameEdgeModel";
    int yaw{ 0 };
    int roll{ 0 };
    String base_name{ "frameEdge" };
    String entity_name;
    for (int fix{ 0 }; fix < 3; ++fix)
    {
        x = 0, y = 3.5, z = 3.5;

        for (int v{ 0 }; v < 4; ++v)
        {
            switch (fix)
            {
            case 0:     // fix x, vary y z    ----
                y = 7 * (v / 2);
                z = 7 * (v % 2);
                break;
            case 1:     // fix y, vary x z    ||||
                x = 3.5 - 7 * (v / 2);
                z = 7 * (v % 2);
                roll = 90;
                break;
            case 2:     // fix z, vary x y    ////
                x = 3.5 - 7 * (v / 2);
                y = 7 * (v % 2);
                yaw = 90;
                roll = 0;
                break;
            }

            sprintf(cframestr_buffer, cframestr, x, y, z, yaw, roll);
            TextInput ti_cframe{ TextInput::FROM_STRING, String{cframestr_buffer} };
            fc_frameEdge["frame"] = Any{ ti_cframe };

            entity_name = base_name + toString(4 * fix + v);
            entities_a[entity_name] = fc_frameEdge;
        }
    }
}

void addCubeAt(Any& entities_a, Point3 position, int face_id, int count)
{
    static char cframestr_buffer[256];
    static char cframestr[]{ "CFrame::fromXYZYPRDegrees(%f, %f, %f, 0., 0., 0.);" };
    static String base_name{ "cube" };

    static Any fc_unit{ Any::TABLE };
    fc_unit.setName("VisibleEntity");
    fc_unit["model"] = "unitModel";

    sprintf(cframestr_buffer, cframestr, position.x, position.y, position.z);
    TextInput ti_cframe{ TextInput::FROM_STRING, String{cframestr_buffer} };
    fc_unit["frame"] = Any{ ti_cframe };
    auto entity_name{ base_name + toString(face_id) + "_" + toString(count++)};
    entities_a[entity_name] = fc_unit;
}

void initFaceGrids(std::vector<FaceGrid>& face_grids)
{
    int width{ 8 };

    face_grids.push_back(FaceGrid{ width, GridPlane::plane_xy, Point3{-3.5, 0., 0.} });
    face_grids.push_back(FaceGrid{ width, GridPlane::plane_yz, Point3{-3.5, 0., 0.} });
    face_grids.push_back(FaceGrid{ width, GridPlane::plane_xz, Point3{-3.5, 0., 0.} });
    face_grids.push_back(FaceGrid{ width, GridPlane::plane_yz, Point3{3.5, 0., 0.} });
    face_grids.push_back(FaceGrid{ width, GridPlane::plane_xy, Point3{-3.5, 0., 7.} });
    face_grids.push_back(FaceGrid{ width, GridPlane::plane_xz, Point3{-3.5, 7., 0.} });
}

void decorateFace(Any& entities_a, FaceGrid& face_grid, int face_id)
{
    int grid_index;
    Point3 world_position;
    Direction dir;
    int fill{ 0 };

    while (fill < face_grid.size() / 4)
    {
        grid_index = getRandInt(0, face_grid.size());

        face_grid.setCursor(grid_index);
        if (face_grid.cursor()->isAvailable())
        {
            face_grid.cursor()->fill();
            world_position = face_grid.cursorWorldPosition();
            addCubeAt(entities_a, world_position, face_id, fill);
            ++fill;

            dir = face_grid.selectExtendDirection();

            // If valid direction, add cubes extending in both the chosen direction and its antiparallel until blocked
            if (dir != Direction::max_directions)
            {
                while (face_grid.cursor()->adj(dir)->isAvailable())
                {
                    face_grid.moveCursor(dir);
                    face_grid.cursor()->fill();
                    world_position = face_grid.cursorWorldPosition();
                    addCubeAt(entities_a, world_position, face_id, fill);
                    ++fill;
                }

                dir = antiparallel(dir);
                face_grid.setCursor(grid_index);
                while (face_grid.cursor()->adj(dir)->isAvailable())
                {
                    face_grid.moveCursor(dir);
                    face_grid.cursor()->fill();
                    world_position = face_grid.cursorWorldPosition();
                    addCubeAt(entities_a, world_position, face_id, fill);
                    ++fill;
                }
            }
        }
    }
}

void buildPC(Any& entities_a)
{
    addFrame(entities_a);

    std::vector<FaceGrid> faces{};
    initFaceGrids(faces);

    for (int face_id{ 0 }; face_id < faces.size(); ++face_id)
    {
        decorateFace(entities_a, faces[face_id], face_id);
    }
}

void buildPCLights(Any& entities_a)
{
    Any sun{ Any::TABLE };

    sun.setName("Light");

    TextInput attenuation{ TextInput::FROM_STRING, "(0, 0, 1);" };
    TextInput bulb_power{ TextInput::FROM_STRING, "Power3(4e+006);" };
    TextInput frame{ TextInput::FROM_STRING, "CFrame::fromXYZYPRDegrees(-21, 167, -30, -164, -77, 77);" };
    TextInput shadow_map_size{ TextInput::FROM_STRING, "Point2int16(2048, 2048);" };

    sun["attenuation"] = Any{ attenuation };
    sun["bulbPower"] = Any{ bulb_power };
    sun["frame"] = Any{ frame };
    sun["shadowMapSize"] = Any{ shadow_map_size };
    sun["spotHalfAngleDegrees"] = 55;
    sun["rectangular"] = true;
    sun["type"] = "SPOT";

    entities_a["sun"] = sun;
}

Any buildPCEntities()
{
    Any entities_a{ Any::TABLE };

    buildPC(entities_a);
    buildPCLights(entities_a);

    return entities_a;
}