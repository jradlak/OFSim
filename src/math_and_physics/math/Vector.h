#pragma once

#include "../MathTypes.h"

struct Vector 
{
    f64 x, y, z;
    
    Vector() : x(0.0), y(0.0), z(0.0) {}
    Vector(f64 v) : x(v), y(v), z(v) {}
    Vector(f64 _x, f64 _y, f64 _z) : x(_x), y(_y), z(_z) {}

    Vector operator+(const Vector& other) const 
    {
        return {x + other.x, y + other.y, z + other.z};
    }

    Vector operator-(const Vector& other) const 
    {
        return {x - other.x, y - other.y, z - other.z};
    }

    Vector operator*(f64 scalar) const 
    {
        return {x * scalar, y * scalar, z * scalar};
    }

    // Dot product function
    f64 dot(const Vector& other) const 
    {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product function
    Vector cross(const Vector& other) const 
    {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    // Magnitude (length) of the vector
    f64 magnitude() const 
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Normalize the vector
    Vector normalize() const 
    {
        f64 mag = magnitude();
        if (mag == 0.0) return *this;   // Avoid division by zero
        return *this * (1.0 / mag);     // Scalar multiplication by the inverse of the magnitude
    }
};