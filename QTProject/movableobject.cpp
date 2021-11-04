#include "movableobject.h"

MovableObject::MovableObject()
{
    transform = QMatrix4x4();
}

void MovableObject::rotate(const float angle, const bool xAxis, const bool yAxis, const bool zAxis)
{
    transform.rotate(angle, xAxis, yAxis, zAxis);
}

void MovableObject::move(const float x, const float y, const float z)
{
    transform.translate(x, y, z);
}

void MovableObject::reset()
{
    transform = QMatrix4x4();
    projection = QMatrix4x4();
}

void MovableObject::setProjection(const float minX, const float maxX, const float minY, const float maxY, const int frontPlane, const int backPlane)
{
    projection.frustum(minX, maxX, minY, maxY, frontPlane, backPlane);
}

const QMatrix4x4 &MovableObject::getTransform() const
{
    return transform;
}

const QVector4D MovableObject::getLocation() const
{
    return QVector4D(1.0f, 1.0f, 1.0f, 1.0f) * transform;
}

const QMatrix4x4 MovableObject::getProjectedTransform() const
{
    return projection * transform;
}

const QVector4D MovableObject::getProjectedLocation() const
{

    return QVector4D(1.0f, 1.0f, 1.0f, 1.0f) * transform * projection;
}
