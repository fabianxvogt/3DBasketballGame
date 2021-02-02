
#ifndef _QUATERNION_H
#define _QUATERNION_H

#define PI			3.14159265358979323846

#include "glm/detail/type_vec.hpp"
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"


class Quaternion
{
public:

	Quaternion();
	Quaternion(float x, float y, float z, float w);
	~Quaternion();

	glm::mat4 CreateMatrix();
	void CreateFromAxisAngle(const float& in_x,
		const float& in_y,
		const float& in_z,
		const float& in_degrees);

	Quaternion operator *(const Quaternion& q);
	Quaternion multiplyVec3(float v[3]);
	Quaternion conjugate();

	float getX();
	float getY();
	float getZ();


private:

	float x,
		y,
		z,
		w;
};


#endif