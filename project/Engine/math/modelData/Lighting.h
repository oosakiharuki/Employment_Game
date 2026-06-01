#pragma once
#include "Vector3.h"
#include "Vector4.h"

/// <summary>
/// 構造体_ライト_放射線
/// </summary>
struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
	int32_t isEnable;
};

/// <summary>
/// 構造体_ライト_ポイントライト
/// </summary>
struct PointLight {
	Vector4 color;
	Vector3 position;
	float intensity;
	float radius;
	float decay;
	float padding[2];
	int32_t isEnable;
};

/// <summary>
/// 構造体_ライト_スポットライト
/// </summary>
struct SpotLight {
	Vector4 color;
	Vector3 position;
	float intensity;
	Vector3 direction;
	float distance;
	float decay;
	float cosAngle;
	float cosFalloffStart;
	float padding[2];
	int32_t isEnable;
};