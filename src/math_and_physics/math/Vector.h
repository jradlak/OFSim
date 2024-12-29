#pragma once

#include "../MathTypes.h"

// 3D vector implementations. At this point only wrapper around glm classes;

class Vector {
public:
    Vector() : vec(0.0, 0.0, 0.0) {}
    Vector(f64 v) : vec(v) {}
    Vector(f64 x, f64 y, f64 z) : vec(x, y, z) {}

    Vector(const glm::dvec3& other) : vec(other) {}

    f64 getX() const { return vec.x; }
    f64 getY() const { return vec.y; }
    f64 getZ() const { return vec.z; }

    void setX(f64 x) { vec.x = x; }
    void setY(f64 y) { vec.y = y; }
    void setZ(f64 z) { vec.z = z; }

    // Function to get the underlying glm::dvec3
    dvec3 getVec() const { return vec; }

    // Function to set the underlying glm::dvec3
    void setVec(const glm::dvec3& newVec) { vec = newVec; }

    Vector operator+(const Vector& other) const 
    {
        return Vector(vec + other.vec);
    }

    Vector operator-(const Vector& other) const 
    {
        return Vector(vec - other.vec);
    }

    Vector operator*(f64 scalar) const 
    {
        return Vector(vec * scalar);
    }

    // Dot product with another vector
    f64 dot(const Vector& other) const 
    {
        return glm::dot(vec, other.vec);
    }

    // Cross product with another vector
    Vector cross(const Vector& other) const 
    {
        return Vector(glm::cross(vec, other.vec));
    }

    // Magnitude of the vector
    f64 magnitude() const 
    {
        return glm::length(vec);
    }

    // Normalize the vector
    void normalize() 
    {
        vec = glm::normalize(vec);
    }

    private:
        dvec3 vec;
};