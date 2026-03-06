#pragma once
#include "MyMath.h"

/// <summary>
/// Primitive
/// </summary>
namespace Primitive {

	ModelData CreateBox();
	ModelData CreatePlane();
	ModelData CreateRing();
	ModelData CreateCylinder();
	ModelData CreateSphere();

	ModelData CreateCone();//円錐
	ModelData CreateBeam();//ビーム

}