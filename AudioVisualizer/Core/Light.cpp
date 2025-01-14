// Includes
#include "Light.h"

Light::Light()
{
}

Light::Light(const Light& copy)
{
}

Light::~Light()
{
}

void Light::SetDiffuseColor(float r, float g, float b, float alpha)
{
	m_diffuseColor = XMFLOAT4(r, g, b, alpha);
	return;
}

void Light::SetDirection(float x, float y, float z)
{
	m_direction = XMFLOAT3(x, y, z);
}

XMFLOAT4 Light::GetDiffuseColor()
{
	return m_diffuseColor;
}

XMFLOAT3 Light::GetDirection()
{
	return m_direction;
}