#pragma once
#include "..\Common\DeviceResources.h"
#include <vector>
#include "Content/ShaderStructures.h"

using namespace demo;
using namespace DirectX;

class Grid {
public:
	Grid(const int& NumberOfCellsPerDirection, const float& Thickness);

	std::vector<VertexPositionColor> CreateData(std::vector<VertexPositionColor>& cubeVertices, const XMVECTORF32& cameraPosition) const;
	const float GetThickness()const;
	const int GetSize()const;

private:
	const int numberOfCellsPerDirection;
	float thickness;
	float cellSize;
};