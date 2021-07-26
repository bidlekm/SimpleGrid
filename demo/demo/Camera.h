#pragma once

using namespace DirectX;
class Camera
{
public:
	Camera(const XMVECTORF32& pos, const XMVECTORF32& at, const XMVECTORF32& up);
	const XMVECTORF32 GetPosition()const;
	const XMVECTORF32 GetLookAt()const;
	const XMVECTORF32 GetUp()const;
private:
	const XMVECTORF32 position;
	const XMVECTORF32 lookAt;
	const XMVECTORF32 up;
};

