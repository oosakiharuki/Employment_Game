#include "Primitive.h"

using namespace MyMath;

namespace Primitive {

	ModelData CreatePlane() {
		ModelData modelData;

		modelData.vertices.push_back({ {1.0f,1.0f,0.0f,1.0f},{0.0f,0.0f},{0.0f,0.0f,1.0f} });
		modelData.vertices.push_back({ {-1.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},{0.0f,0.0f,1.0f} });
		modelData.vertices.push_back({ {1.0f,-1.0f,0.0f,1.0f},{0.0f,1.0f},{0.0f,0.0f,1.0f} });
		modelData.vertices.push_back({ {1.0f,-1.0f,0.0f,1.0f},{0.0f,1.0f},{0.0f,0.0f,1.0f} });
		modelData.vertices.push_back({ {-1.0f,1.0f,0.0f,1.0f},{1.0f,0.0f},{0.0f,0.0f,1.0f} });
		modelData.vertices.push_back({ {-1.0f,-1.0f,0.0f,1.0f},{1.0f,1.0f},{0.0f,0.0f,1.0f} });

		return modelData;
	}



	ModelData CreateBox() {
		VertexData vertexs[24];
		ModelData modelData;

		//右面
		vertexs[0].position = { 1.0f,1.0f,1.0f,1.0f };
		vertexs[1].position = { 1.0f,1.0f,-1.0f,1.0f };
		vertexs[2].position = { 1.0f,-1.0f,1.0f,1.0f };
		vertexs[3].position = { 1.0f,-1.0f,-1.0f,1.0f };

		modelData.vertices.push_back(vertexs[0]);
		modelData.vertices.push_back(vertexs[1]);
		modelData.vertices.push_back(vertexs[2]);

		modelData.vertices.push_back(vertexs[2]);
		modelData.vertices.push_back(vertexs[1]);
		modelData.vertices.push_back(vertexs[3]);


		//左面
		vertexs[4].position = { -1.0f,1.0f,-1.0f,1.0f };
		vertexs[5].position = { -1.0f,1.0f,1.0f,1.0f };
		vertexs[6].position = { -1.0f,-1.0f,-1.0f,1.0f };
		vertexs[7].position = { -1.0f,-1.0f,1.0f,1.0f };

		modelData.vertices.push_back(vertexs[4]);
		modelData.vertices.push_back(vertexs[5]);
		modelData.vertices.push_back(vertexs[6]);

		modelData.vertices.push_back(vertexs[6]);
		modelData.vertices.push_back(vertexs[5]);
		modelData.vertices.push_back(vertexs[7]);

		//前面
		vertexs[8].position = { -1.0f,1.0f,1.0f,1.0f };
		vertexs[9].position = { 1.0f,1.0f,1.0f,1.0f };
		vertexs[10].position = { -1.0f,-1.0f,1.0f,1.0f };
		vertexs[11].position = { 1.0f,-1.0f,1.0f,1.0f };

		modelData.vertices.push_back(vertexs[8]);
		modelData.vertices.push_back(vertexs[9]);
		modelData.vertices.push_back(vertexs[10]);

		modelData.vertices.push_back(vertexs[10]);
		modelData.vertices.push_back(vertexs[9]);
		modelData.vertices.push_back(vertexs[11]);

		//後ろ面
		vertexs[12].position = { -1.0f,1.0f,-1.0f,1.0f };
		vertexs[13].position = { 1.0f,1.0f,-1.0f,1.0f };
		vertexs[14].position = { -1.0f,-1.0f,-1.0f,1.0f };
		vertexs[15].position = { 1.0f,-1.0f,-1.0f,1.0f };

		modelData.vertices.push_back(vertexs[12]);
		modelData.vertices.push_back(vertexs[13]);
		modelData.vertices.push_back(vertexs[14]);

		modelData.vertices.push_back(vertexs[14]);
		modelData.vertices.push_back(vertexs[13]);
		modelData.vertices.push_back(vertexs[15]);

		//上面
		vertexs[16].position = { -1.0f,1.0f,-1.0f,1.0f };
		vertexs[17].position = { -1.0f,1.0f,1.0f,1.0f };
		vertexs[18].position = { 1.0f,1.0f,-1.0f,1.0f };
		vertexs[19].position = { 1.0f,1.0f,1.0f,1.0f };

		modelData.vertices.push_back(vertexs[16]);
		modelData.vertices.push_back(vertexs[17]);
		modelData.vertices.push_back(vertexs[18]);

		modelData.vertices.push_back(vertexs[18]);
		modelData.vertices.push_back(vertexs[17]);
		modelData.vertices.push_back(vertexs[19]);

		//下面
		vertexs[20].position = { -1.0f,-1.0f,-1.0f,1.0f };
		vertexs[21].position = { -1.0f,-1.0f,1.0f,1.0f };
		vertexs[22].position = { 1.0f,-1.0f,-1.0f,1.0f };
		vertexs[23].position = { 1.0f,-1.0f,1.0f,1.0f };

		modelData.vertices.push_back(vertexs[20]);
		modelData.vertices.push_back(vertexs[21]);
		modelData.vertices.push_back(vertexs[22]);

		modelData.vertices.push_back(vertexs[22]);
		modelData.vertices.push_back(vertexs[21]);
		modelData.vertices.push_back(vertexs[23]);

		return modelData;
	}

	ModelData CreateRing() {
		ModelData modelData;

		const uint32_t kRingDivide = 32;
		const float kOuterRadius = 1.0f;
		const float kInnerRadius = 0.2f;
		const float radianPreDevice = 2.0f * std::numbers::pi_v<float> / float(kRingDivide);

		for (uint32_t index = 0; index < kRingDivide; ++index) {
			float sin = std::sin(index * radianPreDevice);
			float cos = std::cos(index * radianPreDevice);

			float sinNext = std::sin((index + 1) * radianPreDevice); // ベジェ曲線のイメージ
			float cosNext = std::cos((index + 1) * radianPreDevice);

			float u = float(index) / float(kRingDivide);
			float uNext = float(index + 1) / float(kRingDivide);

			VertexData vertexData1, vertexData2, vertexData3, vertexData4;

			vertexData1.position = { -sin * kOuterRadius, cos * kOuterRadius,0.0f,1.0f };
			vertexData1.texcoord = { u,0.0f };

			vertexData2.position = { -sinNext * kOuterRadius, cosNext * kOuterRadius, 0.0f,1.0f };
			vertexData2.texcoord = { uNext,0.0f };

			vertexData3.position = { -sin * kInnerRadius, cos * kInnerRadius, 0.0f,1.0f };
			vertexData3.texcoord = { u,1.0f };

			vertexData4.position = { -sinNext * kInnerRadius, cosNext * kInnerRadius, 0.0f,1.0f };
			vertexData4.texcoord = { uNext,1.0f };


			modelData.vertices.push_back(vertexData1);
			modelData.vertices.push_back(vertexData2);
			modelData.vertices.push_back(vertexData3);

			modelData.vertices.push_back(vertexData3);
			modelData.vertices.push_back(vertexData2);
			modelData.vertices.push_back(vertexData4);

		}

		return modelData;
	}



	ModelData CreateCylinder() {
		ModelData modelData;

		const uint32_t kCylinderDivide = 32;
		const float kTopRadius = 1.0f;
		const float kBottomRadius = 1.0f;
		const float kHeight = 3.0f;
		const float radianPreDevice = 2.0f * std::numbers::pi_v<float> / float(kCylinderDivide);

		for (uint32_t index = 0; index < kCylinderDivide; ++index) {
			float sin = std::sin(index * radianPreDevice);
			float cos = std::cos(index * radianPreDevice);

			float sinNext = std::sin((index + 1) * radianPreDevice); // ベジェ曲線のイメージ
			float cosNext = std::cos((index + 1) * radianPreDevice);

			float u = float(index) / float(kCylinderDivide);
			float uNext = float(index + 1) / float(kCylinderDivide);

			VertexData vertexData1, vertexData2, vertexData3, vertexData4, vertexData5, vertexData6;

			vertexData1.position = { -sin * kTopRadius,kHeight, cos * kTopRadius,1.0f };
			vertexData1.texcoord = { u,0.0f };
			vertexData1.normal = { -sin,0.0f,cos };

			vertexData2.position = { -sinNext * kTopRadius,kHeight, cosNext * kTopRadius,1.0f };
			vertexData2.texcoord = { uNext,0.0f };
			vertexData2.normal = { -sinNext,0.0f,cosNext };

			vertexData3.position = { -sin * kBottomRadius,0.0f, cos * kBottomRadius,1.0f };
			vertexData3.texcoord = { u,1.0f };
			vertexData3.normal = { -sin,0.0f,cos };

			vertexData4.position = { -sin * kBottomRadius,0.0f, cos * kBottomRadius,1.0f };
			vertexData4.texcoord = { u,1.0f };
			vertexData4.normal = { -sin,0.0f,cos };

			vertexData5.position = { -sinNext * kTopRadius,kHeight, cosNext * kTopRadius,1.0f };
			vertexData5.texcoord = { uNext,0.0f };
			vertexData5.normal = { -sinNext,0.0f,cosNext };

			vertexData6.position = { -sinNext * kBottomRadius,0.0f, cosNext * kBottomRadius,1.0f };
			vertexData6.texcoord = { uNext,1.0f };
			vertexData6.normal = { -sinNext,0.0f,cosNext };


			modelData.vertices.push_back(vertexData1);
			modelData.vertices.push_back(vertexData2);
			modelData.vertices.push_back(vertexData3);

			modelData.vertices.push_back(vertexData3);
			modelData.vertices.push_back(vertexData2);
			modelData.vertices.push_back(vertexData6);

		}

		return modelData;
	}

	ModelData CreateSphere() {

		ModelData modelData;

		const uint32_t kSubdivision = 16;

		float pi = float(M_PI);

		const float kLonEvery = pi * 2.0f / float(kSubdivision);
		const float kLatEvery = pi / float(kSubdivision);


		VertexData vertexDataBkaraA[kSubdivision]{};

		VertexData vertexDataCkaraA[kSubdivision]{};

		VertexData vertexDataDkaraA[kSubdivision][kSubdivision]{};

		VertexData vertexDataDkaraC[kSubdivision]{};
		VertexData vertexDataDkaraB[kSubdivision]{};


		for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
			float lat = -pi / 2.0f + kLatEvery * latIndex;//緯度 シ－タ

			for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {

				uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
				float lon = lonIndex * kLonEvery;//経度　ファイ


				VertexData vertA{};
				vertA.position =
				{
					std::cos(lat) * std::cos(lon),
					std::sin(lat),
					std::cos(lat) * std::sin(lon),
					1.0f
				};
				vertA.texcoord =
				{
					float(lonIndex) / float(kSubdivision),
					1.0f - float(latIndex) / float(kSubdivision)
				};
				vertA.normal = {
					0.0f,0.0f,-1.0f
				};


				VertexData vertB{};
				vertB.position =
				{
					std::cos(lat + kLatEvery) * std::cos(lon),
					std::sin(lat + kLatEvery),
					std::cos(lat + kLatEvery) * std::sin(lon)
					,1.0f
				};
				vertB.texcoord =
				{
					float(lonIndex) / float(kSubdivision),
					1.0f - float(latIndex + 1) / float(kSubdivision)
				};
				vertB.normal = {
					0.0f,0.0f,-1.0f
				};


				VertexData vertC{};
				vertC.position =
				{
					std::cos(lat) * std::cos(lon + kLonEvery),
					std::sin(lat),
					std::cos(lat) * std::sin(lon + kLonEvery),
					1.0f
				};
				vertC.texcoord =
				{
					float(lonIndex + 1) / float(kSubdivision),
					1.0f - float(latIndex) / float(kSubdivision)
				};
				vertC.normal = {
					0.0f,0.0f,-1.0f
				};


				VertexData vertD{};
				vertD.position =
				{
					std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery),
					std::sin(lat + kLatEvery),
					std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery),
					1.0f
				};
				vertD.texcoord =
				{
					float(lonIndex + 1) / float(kSubdivision),
					1.0f - float(latIndex + 1) / float(kSubdivision)
				};
				vertD.normal = {
					0.0f,0.0f,-1.0f
				};




				//最初点
				modelData.vertices.push_back(vertA);
				modelData.vertices.push_back(vertB);
				modelData.vertices.push_back(vertC);

				modelData.vertices.push_back(vertC);
				modelData.vertices.push_back(vertB);
				modelData.vertices.push_back(vertD);

			}

		}

		for (uint32_t index = 0; index < kSubdivision * kSubdivision * 6; index++) {
			modelData.vertices[index].normal.x = modelData.vertices[index].position.x;
			modelData.vertices[index].normal.y = modelData.vertices[index].position.y;
			modelData.vertices[index].normal.z = modelData.vertices[index].position.z;
		}

		return modelData;
	}

}