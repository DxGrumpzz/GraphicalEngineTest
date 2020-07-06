#pragma once

#include <exception>
#include <math.h>


class Vector2D
{
public:

    float X = 0;
    float Y = 0;


public:

    Vector2D(float x = 0.0f, float y = 0.0f) :
        X(x),
        Y(y)
    {
    }


public:

    float Length() const
    {
        return sqrtf(LengthSquare());
    };

    int LengthSquare() const
    {
        return (X * X) + (Y * Y);
    };


    Vector2D& Normalize()
    {
        float length = Length();

        if (length != 0.0f)
        {
            X *= (1.0f / length);
            Y *= (1.0f / length);
        };

        return *this;
    };


    Vector2D Normalized() const
    {
        float length = Length();

        if (length != 0)
        {
            Vector2D copy = { X,  Y };

            copy.X /= length;
            copy.Y /= length;

            return copy;
        }
        else
            throw std::exception("Invalid length division");
    };


    Vector2D& Scale(float scalar)
    {
        X *= scalar;
        Y *= scalar;

        return *this;
    };


    void Rotate(float angle)
    {
        float cosResult = cos(angle);
        float sinResult = sin(angle);

        float angledX = X * cosResult - Y * sinResult;
        float angledY = X * sinResult + Y * cosResult;

        X = angledX;
        Y = angledY;
    };

    void RotateDeg(float degrees)
    {
        float radians = degrees * 3.14159265 / 180;

        Rotate(radians);
    };


    #pragma region Operators

public:

    Vector2D operator - (const Vector2D& rightVector) const
    {
        return Vector2D({ X - rightVector.X,  Y - rightVector.Y });
    };

    Vector2D& operator -= (const Vector2D& rightVector)
    {
        X -= rightVector.X;
        Y -= rightVector.Y;

        return *this;
    };


    Vector2D operator + (const Vector2D& rightVector) const
    {
        return Vector2D({ X + rightVector.X,  Y + rightVector.Y });
    };

    Vector2D& operator += (const Vector2D& rightVector)
    {
        X += rightVector.X;
        Y += rightVector.Y;

        return *this;
    };



    Vector2D operator * (const Vector2D& rightVector) const
    {
        return Vector2D({ X * rightVector.X,  Y * rightVector.Y });
    };

    Vector2D& operator *= (const Vector2D& rightVector)
    {
        X *= rightVector.X;
        Y *= rightVector.Y;

        return *this;
    };


    Vector2D operator * (float value) const
    {
        return Vector2D({ X * value,  Y * value });
    };

    Vector2D& operator *= (float value)
    {
        X *= value;
        Y *= value;

        return *this;
    };


    #pragma endregion

};