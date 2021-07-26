#include "pch.h"
#include "Camera.h"


Camera::Camera(const XMVECTORF32& pos, const XMVECTORF32& at, const XMVECTORF32& up) : position(pos), lookAt(at), up(up) {}
const XMVECTORF32 Camera::GetPosition() const{ return position; }
const XMVECTORF32 Camera::GetLookAt() const { return lookAt; }
const XMVECTORF32 Camera::GetUp() const { return up; }