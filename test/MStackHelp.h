/*
 * RenderingHelper.h -> renamed MStack.h (Z.J.W.)
 * A means of replacing openGL matrix stack uses glm 
 *
 *  Created on: Jul 28, 2011
 *      Author: Wyatt and Evan
 */

#ifndef RenderingHelper_H_
#define RenderingHelper_H_

#include <stack>

 #define GLM_FORCE_RADIANS

#include "glm/glm.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/gtx/component_wise.hpp"

#include "glm/gtc/matrix_access.hpp"
#include "glm/gtc/matrix_integer.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_transform.hpp"

using glm::ivec2;
using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::mat2;
using glm::mat3;
using glm::mat4;

struct RenderingHelper {
	mat4 modelViewMatrix;
	std::stack<mat4> modelViewMatrixStack;

	mat4 projectionMatrix;
	std::stack<mat4> projectionMatrixStack;

	mat4 *currentMatrix;
	std::stack<mat4> *currentMatrixStack;

public:
	RenderingHelper();
	virtual ~RenderingHelper();

	void useModelViewMatrix();
	void useProjectionMatrix();

	void pushMatrix();
	void popMatrix();

	void loadIdentity();

	void translate(const vec3 &offset);
	void scale(float x, float y, float z);
	void scale(float size);
	void rotate(float angle, const vec3 &axis);

	void multMatrix(const mat4 &matrix);
	const mat4 &getMatrix();

	void ortho(float left, float right, float bottom, float top, float zNear, float zFar);
	void frustum(float left, float right, float bottom, float top, float zNear, float zFar);
	void lookAt(vec3 eye, vec3 target, vec3 up);
};

#endif /* RenderingHelper_H_ */
