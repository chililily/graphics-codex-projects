#pragma once

#include "..\source\OFFdata.h"
#include <cassert>

namespace OFFdataTest
{
	bool TestWrite(string filepath)
	{
		OFFdata test_cube{ filepath, 8, 6, 12, 4 };
		test_cube.appendComment("test_cube.off");

		int x;
		int y;
		int z;
		for (int i{ 0 }; i < 8; ++i)
		{
			x = pow(-1, i / 4) * 0.5;
			y = pow(-1, i / 2) * 0.5;
			z = pow(-1, i % 2) * 0.5;
			test_cube.appendVertex(x, y, z);
		}

		std::vector<std::vector<int>> face_vertices{
			{0, 1, 2, 3},
			{0, 1, 4, 5},
			{0, 2, 4, 6},
			{1, 3, 5, 7},
			{2, 3, 6, 7},
			{4, 5, 6, 7}
		};

		for (auto f_vs : face_vertices)
		{
			test_cube.appendFace(f_vs);
		}

		bool success = test_cube.write();
		return success;
	}

	//bool TestRead(string filepath)
	//{
	//	std::fstream file;
	//	file.open(filepath, std::ios::in);

	//	// check file contents

	//	file.close();
	//	return true;
	//}
}