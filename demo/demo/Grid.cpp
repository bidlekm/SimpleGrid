#include "pch.h"
#include "Grid.h"

Grid::Grid(const int& NumberOfCellsPerDirection , const float& Thickness) : numberOfCellsPerDirection(NumberOfCellsPerDirection), thickness(Thickness) {
	cellSize = 1.0f / (NumberOfCellsPerDirection / 10.0f);
}
const float Grid::GetThickness() const { return this->thickness; }
const int Grid::GetSize() const { return this->numberOfCellsPerDirection; }


std::vector<VertexPositionColor> Grid::CreateData(std::vector<VertexPositionColor>& cubeVertices, const XMVECTORF32& cameraPosition) const {
	float posX, posZ;
	unsigned int index = 0;
	cubeVertices.resize((2 * numberOfCellsPerDirection + 1) * 2 * 2);

	int xOffset = (int)(cameraPosition.f[0] / cellSize);
	int zOffset = (int)(cameraPosition.f[2] / cellSize);
	
	//z parallels
	for (int i = -numberOfCellsPerDirection + xOffset; i <= numberOfCellsPerDirection + xOffset; ++i)
	{	
		posX = (float)i * cellSize;
		posZ = numberOfCellsPerDirection * cellSize + cameraPosition[2];
		cubeVertices[index++].pos = XMFLOAT3(posX, 0.0f, posZ);

		posX = (float)i * cellSize;
		posZ = -numberOfCellsPerDirection * cellSize + cameraPosition[2];
		cubeVertices[index++].pos = XMFLOAT3(posX, 0.0f, posZ);
	}

	//x parallels
	for (int j = -numberOfCellsPerDirection + zOffset; j <= numberOfCellsPerDirection + zOffset; ++j)
	{
		posX = numberOfCellsPerDirection * cellSize + cameraPosition[0];
		posZ = (float)j * cellSize;
		cubeVertices[index++].pos = XMFLOAT3(posX, 0.0f, posZ);

		posX = -numberOfCellsPerDirection * cellSize + cameraPosition[0];
		posZ = (float)j * cellSize;
		cubeVertices[index++].pos = XMFLOAT3(posX, 0.0f, posZ);
	}

	return cubeVertices;
}