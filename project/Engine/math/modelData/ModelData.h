#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

#include <string>
#include <vector>

/// <summary>
/// 構造体_MaterialData
/// </summary>
struct MaterialData {
	std::string textureFilePath;
	uint32_t textureIndex;
	Matrix4x4 uvTransform;
	Vector4 materialColor;
};

/// <summary>
/// 構造体‗VertexData
/// </summary>
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

/// <summary>
/// 構造体_ModelData
/// </summary>
struct ModelData {
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	MaterialData materialData;
};