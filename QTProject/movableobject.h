#ifndef MOVABLEOBJECT_H
#define MOVABLEOBJECT_H

#include <QMatrix4x4>

class MovableObject
{
public:
    MovableObject();

    void rotate(const float angle, const bool xAxis, const bool yAxis, const bool zAxis);
    void move(const float x, const float y, const float z);
    void reset();

    void setProjection(const float minX, const float maxX, const float minY, const float maxY, const int frontPlane, const int backPlane);

    const QMatrix4x4 &getTransform() const;
    const QMatrix4x4 getProjectedTransform() const;
    const QVector4D getLocation() const;
    const QVector4D getProjectedLocation() const;
    QMatrix4x4 projection;

private:
    QMatrix4x4 transform;

};

#endif // MOVABLEOBJECT_H
