#include "pch.h"
#include "CppUnitTest.h"

#include "..\source\OFFdata.h"
#include <cmath>
#include <cstring>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(OFFdataTests)
	{
	public:
		TEST_METHOD(setVertices)
		{
			OFFdata test;

			std::vector<Point3> verts{ {2., -1.3, 8.2} };
			test.setVertices(verts);

			Assert::AreEqual(static_cast<int>(test.getVertices()->size()), 1);
		}

		TEST_METHOD(NonDefault_Constructor)
		{
			string filepath{ "test.off" };
			OFFdata test_cube{ filepath, 8, 6, 12, 4 };

			Assert::AreEqual(*test_cube.getFilepath(), static_cast<string>("test.off"));
			Assert::AreEqual(test_cube.getnVertices(), 8);
			Assert::AreEqual(test_cube.getnFaces(), 6);
			Assert::AreEqual(test_cube.getnEdges(), 12);
			Assert::AreEqual(test_cube.getnVertsPerFace(), 4);
		}
	};
}
