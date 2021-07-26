#include "staircase.h"

template <typename T>
String toString(T x)
{
    return String{ std::to_string(x) };
}

Any buildModels()
{
    Any models_a{ Any::TABLE };
    
    Any crate_spec{ Any::TABLE };
    crate_spec.setName("ArticulatedModel::Specification");
    crate_spec["filename"] = "model/crate/crate.obj";

    Any preprocess_a{ Any::ARRAY, "", "{}", ';'};
    TextInput ti_setMaterial{ TextInput::FROM_STRING, "setMaterial(all(), \"material/roughcedar/roughcedar.UniversalMaterial.Any\");" };
    TextInput ti_transform{ TextInput::FROM_STRING, "transformGeometry(all(), Matrix4::scale(1.2, 0.08, 0.4));" };
    preprocess_a.append(Any{ ti_setMaterial });
    preprocess_a.append(Any{ ti_transform });
    crate_spec["preprocess"] = preprocess_a;

    models_a["stepModel"] = crate_spec;

    return models_a;
}

void buildStaircase(Any& entities_a)
{
    Any stair{ Any::TABLE };

    String base_name{ "stair" };
    String entity_name;
    char framestr_buffer[64];
    char framestr[]{ "CFrame::fromXYZYPRDegrees(%f, %f, %f, %i, 0., 0.);" };

    double x_translate;
    double y_initial{ 4 };
    double y_delta;
    double z_initial{ -10 };
    double z_delta;
    int yaw;
    for (int i{ 0 }; i < 160; ++i)
    {
        stair.setName("VisibleEntity");
        stair["model"] = "stepModel";

        y_delta = -0.08 * i;
        yaw = (i * 12) % 360;
        x_translate = 0.85*cos(toRadians(yaw));
        z_delta = -0.85*sin(toRadians(yaw));
        sprintf(framestr_buffer, framestr, x_translate, y_initial + y_delta, z_initial + z_delta, yaw);
        TextInput ti_frame{ TextInput::FROM_STRING, String{framestr_buffer} };
        stair["frame"] = Any{ ti_frame };

        entity_name = base_name + toString(i);
        entities_a[entity_name] = stair;
    }
}

void buildLights(Any& entities_a)
{
    Any sun{ Any::TABLE };

    sun.setName("Light");

    TextInput attenuation{ TextInput::FROM_STRING, "(0, 0, 1);" };
    TextInput bulb_power{ TextInput::FROM_STRING, "Power3(4e+006);" };
    TextInput frame{ TextInput::FROM_STRING, "CFrame::fromXYZYPRDegrees(-21, 167, -30, -164, -77, 77);" };
    TextInput shadow_map_size{ TextInput::FROM_STRING, "Vector2int16(2048, 2048);" };

    sun["attenuation"] = Any{ attenuation };
    sun["bulbPower"] = Any{ bulb_power };
    sun["frame"] = Any{ frame };
    sun["shadowMapSize"] = Any{ shadow_map_size };
    sun["spotHalfAngleDegrees"] = 55;
    sun["rectangular"] = true;
    sun["type"] = "SPOT";

    entities_a["sun"] = sun;
}

Any buildEntities()
{
    Any entities_a{ Any::TABLE };
    
    buildStaircase(entities_a);
    buildLights(entities_a);

    return entities_a;
}

void createStaircaseAny()
{
    Any staircase_any{ Any::TABLE };
    staircase_any["name"] = "Staircase";

    staircase_any["models"] = buildModels();
    staircase_any["entities"] = buildEntities();

    staircase_any.save("../data-files/scene/staircase.Scene.Any");
}