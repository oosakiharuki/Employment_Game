#pragma once
#include "AABB.h"
#include "Lighting.h"
#include "Matrix4x4.h"
#include "ModelData.h"
#include "Quaternion.h"
#include "Segment.h"
#include "Transform.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include "AnimationMath.h"
#include "CollisionMath.h"
#include "Easing.h"
#include "LengthMath.h"
#include "Lerp.h"
#include "MatrixMath.h"

#include <wrl/client.h>
#include <d3d12.h>

#include <cassert>
#include <cmath>

/// <summary>
///	構造体__Material
/// </summary>
struct Material {
	Vector4 color;
	bool enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	float shininess;
	float environmentCoefficient;
};

struct CameraForGPU {
	Vector3 worldPosition;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};
