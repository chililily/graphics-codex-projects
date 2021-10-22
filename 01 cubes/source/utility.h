#pragma once

#include <G3D/G3D.h>
#include <array>
#include <iterator>
#include <math.h>
#include <memory>
#include <random>       // std::mt19937

using namespace G3D;

template <typename T>
String toString(T x)
{
    return String{ std::to_string(x) };
}

int getRandInt(int min, int max);

void createAny(const String& filepath, const String& name, const Any& models, const Any& entities);