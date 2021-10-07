#include "utility.h"

int getRandInt(int min, int max)
{
    static std::mt19937 mersenne{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
    static std::uniform_int_distribution rng{ min, max };

    return rng(mersenne);
}

void createAny(const String& filepath, const String& name, const Any& models, const Any& entities)
{
    Any any{ Any::TABLE };
    any["name"] = name;

    any["models"] = models;
    any["entities"] = entities;

    any.save(filepath);
}