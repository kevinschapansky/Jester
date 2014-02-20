/*
 * RenderingHelper.cpp -> renamed MStackHelp.cpp (ZJW)
 * A useful replacement of OpenGL matrix stack using glm
 *
 *  Created on: Jul 28, 2011
 *      Author: Wyatt and Evan
 */

#include "MStackHelp.h"
#include "glm/gtx/transform.hpp"

RenderingHelper::RenderingHelper() :
        modelViewMatrix(1),
        projectionMatrix(1),
        currentMatrix(NULL),
        currentMatrixStack(NULL) { }

RenderingHelper::~RenderingHelper() { }

void RenderingHelper::useModelViewMatrix()
{
	currentMatrix = &modelViewMatrix;
	currentMatrixStack = &modelViewMatrixStack;
}

void RenderingHelper::useProjectionMatrix()
{
	currentMatrix = &projectionMatrix;
	currentMatrixStack = &projectionMatrixStack;
}

void RenderingHelper::pushMatrix()
{
	currentMatrixStack->push(*currentMatrix);
}

void RenderingHelper::popMatrix()
{
	assert(!currentMatrixStack->empty());

	*currentMatrix = currentMatrixStack->top();
	currentMatrixStack->pop();
}

void RenderingHelper::loadIdentity()
{
	*currentMatrix = glm::mat4(1);
}

void RenderingHelper::translate(const vec3 &offset)
{
	*currentMatrix *= glm::translate(offset);
}

void RenderingHelper::scale(float x, float y, float z)
{
	*currentMatrix *= glm::scale(glm::vec3(x, y, z));
}

void RenderingHelper::scale(float size)
{
	*currentMatrix *= glm::scale(glm::vec3(size, size, size));
}

void RenderingHelper::rotate(float angle, const vec3 &axis)
{
	//*currentMatrix = glm::rotate_slow(*currentMatrix, angle, axis);
	//*currentMatrix *= glm::gtx::transform::rotate(angle, axis);
	*currentMatrix *= glm::rotate(angle, axis);
}

void RenderingHelper::multMatrix(const glm::mat4 &matrix)
{
	*currentMatrix *= matrix;
}

void RenderingHelper::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
    *currentMatrix *= glm::ortho(left, right, bottom, top, zNear, zFar);
}

void RenderingHelper::frustum(float left, float right, float bottom, float top, float zNear, float zFar)
{
	*currentMatrix *= glm::frustum(left, right, bottom, top, zNear, zFar);
}

void RenderingHelper::lookAt(vec3 eye, vec3 target, vec3 up)
{
	*currentMatrix *= glm::lookAt(eye, target, up);
}

const glm::mat4 &RenderingHelper::getMatrix()
{
	return *currentMatrix;
}
