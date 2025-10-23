#pragma once
#include "MyMath.h"

/// <summary>
/// Primitive
/// </summary>
enum PrimitiveType {
	box,
	plane,
	ring,
	cylineder,
	sphere,
	cone
};

namespace Primitive {

	ModelData CreateBox();
	ModelData CreatePlane();
	ModelData CreateRing();
	ModelData CreateCylinder();
	ModelData CreateSphere();

	ModelData CreateCone();//円錐

}