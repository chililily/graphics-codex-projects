#include "fancyCube.h"

Any buildFCModels()
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
    TextInput frame_transform{ TextInput::FROM_STRING, "transformGeometry(all(), Matrix4::scale(9, 1, 1));" };
    frame_preprocess.append(Any{ frame_setMaterial });
    frame_preprocess.append(Any{ frame_transform });
    frame_spec["preprocess"] = frame_preprocess;

    models_a["frameEdgeModel"] = frame_spec;

    return models_a;
}

// Build cube frame (edges)
void addFrame(Any& entities_a)
{
    char cframestr_buffer[64];
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
        x = 0, y = 4, z = 4.5;

        for (int v{ 0 }; v < 4; ++v)
        {
            switch (fix)
            {
            case 0:     // fix x, vary y z    ----
                y = 8 * (v / 2);
                z = 9 * (v % 2);
                break;
            case 1:     // fix y, vary x z    ||||
                x = 4.5 - 9 * (v / 2);
                z = 9 * (v % 2);
                roll = 90;
                break;
            case 2:     // fix z, vary x y    ////
                x = 4.5 - 9 * (v / 2);
                y = 8 * (v % 2);
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
    static char cframestr_buffer[64];
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

std::vector<FaceGrid> initFaceGrids()
{
    int width{ 7 };

    std::vector<FaceGrid> face_grids{
        FaceGrid{ width, GridPlane::plane_xy, Point3{-3.5, 1, 0} },
    FaceGrid{ width, GridPlane::plane_yz, Point3{-4.5, 1, 1} },
    FaceGrid{ width, GridPlane::plane_xz, Point3{-3.5, 0, 1} },
    FaceGrid{ width, GridPlane::plane_yz, Point3{4.5, 1, 1} },
    FaceGrid{ width, GridPlane::plane_xy, Point3{-3.5, 1, 9} },
    FaceGrid{ width, GridPlane::plane_xz, Point3{-3.5, 8, 1} }
    };

    return face_grids;
}

void decorateFace(Any& entities_a, FaceGrid& face_grid, int face_id)
{
    auto origin{ face_grid.origin() };
    double x{ origin.x };
    double y{ origin.y };
    double z{ origin.z };
    Point2 grid_position;
    Point3 world_position;
    int fill{ 0 };

    while (fill < 32)
    {
        // Randomly select a grid_position on the cube face
        grid_position = face_grid.toCoordinates(getRandInt(0, face_grid.size()));

        // If selected grid_position is empty...
        if (face_grid.at(grid_position) == GridUnit::empty)
        {
            world_position = face_grid.worldPositionAt(grid_position);
            addCubeAt(entities_a, world_position, face_id, fill++);
            face_grid.fill(grid_position);

            // Select which directions to extend
            face_grid.selectDirections(grid_position);

            // Add cubes extending in each unblocked cardinal direction until another object is hit
            for (auto dir_flag : face_grid.flagset().vector())
            {
                if (!(dir_flag.isBlocked()))
                {
                    while (face_grid.at(face_grid.move(grid_position, dir_flag.dir())) == GridUnit::empty)
                    {
                        world_position = face_grid.worldPositionAt(grid_position);
                        addCubeAt(entities_a, world_position, face_id, fill++);
                        face_grid.fill(grid_position);
                    }
                }
            }
        }
    }
}

void buildFC(Any& entities_a)
{
    addFrame(entities_a);

    std::vector<FaceGrid> faces { initFaceGrids() };

    int face_id{ 0 };
    for (auto &face : faces)
    {
        decorateFace(entities_a, face, face_id++);
    }
}

void buildFCLights(Any& entities_a)
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

Any buildFCEntities()
{
    Any entities_a{ Any::TABLE };

    buildFC(entities_a);
    buildFCLights(entities_a);

    return entities_a;
}