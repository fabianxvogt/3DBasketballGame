#include "quaternion.h"
#include <cmath>
#include "glm/detail/type_vec.hpp"
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

Quaternion::Quaternion()
	: x(0.0f), y(0.0f), z(0.0f), w(1.0f)
{

}Quaternion::Quaternion(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w)
{

}

Quaternion::~Quaternion()
{

}

float Quaternion::getX() {
	return x;
}
float Quaternion::getY() {
	return y;
}
float Quaternion::getZ() {
	return z;
}

void Quaternion::CreateFromAxisAngle(const float& in_x, const float& in_y, const float& in_z, const float& in_degrees)
{

	float angle = float((in_degrees / 180.0f) * PI);
	float result = float(sin(angle / 2.0f));
	w = float(cos(angle / 2.0f));

	// Calculate the x, y and z of the quaternion
	x = float(in_x * result);
	y = float(in_y * result);
	z = float(in_z * result);
}

glm::mat4 Quaternion::CreateMatrix()
{
	glm::mat4 rot = glm::mat4();
		// First row
	rot[0].x = 1.0f - 2.0f * (y * y + z * z);
	rot[0].y = 2.0f * (x * y - w * z);
	rot[0].z = 2.0f * (x * z + w * y);
	rot[0].w = 0.0f;

		// Second row
	rot[1].x = 2.0f * (x * y + w * z);
	rot[1].y = 1.0f - 2.0f * (x * x + z * z);
	rot[1].z = 2.0f * (y * z - w * x);
	rot[1].w = 0.0f;

		// Third row
	rot[2].x = 2.0f * (x * z - w * y);
	rot[2].y = 2.0f * (y * z + w * x);
	rot[2].z = 1.0f - 2.0f * (x * x + y * y);
	rot[2].w = 0.0f;

		// Fourth row
	rot[3].x = 0;
	rot[3].y = 0;
	rot[3].z = 0;
	rot[3].w = 1.0f;
	
	return rot;
}

Quaternion Quaternion::operator *(const Quaternion& q)
{
	Quaternion r;

	r.w = w * q.w - x * q.x - y * q.y - z * q.z;
	r.x = w * q.x + x * q.w + y * q.z - z * q.y;
	r.y = w * q.y + y * q.w + z * q.x - x * q.z;
	r.z = w * q.z + z * q.w + x * q.y - y * q.x;

	return r;
}
Quaternion Quaternion::multiplyVec3(float v[3])
{
	Quaternion r;

	r.w = -x * v[0] - y * v[1] - z * v[2];
	r.x =  w * v[0] + y * v[2] - z * v[1];
	r.y =  w * v[1] + z * v[0] - x * v[2];
	r.z =  w * v[2] + x * v[1] - y * v[0];

	return r;
}


Quaternion Quaternion::conjugate() {
	return Quaternion(-x, -y, -z, w);
}