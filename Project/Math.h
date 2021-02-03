#pragma once
#include <cassert>
#include <cmath>
#include <numbers>
#include <SFML/System/Vector2.hpp>

namespace Math
{
    [[nodiscard]] inline float Length(sf::Vector2f vector)
    {
        return sqrtf(powf(vector.x, 2) + powf(vector.y, 2));
    }

    [[nodiscard]] inline sf::Vector2f Normalize(sf::Vector2f vector)
    {
        return vector / Length(vector);
    }

    [[nodiscard]] inline float Dot(sf::Vector2f first, sf::Vector2f second) noexcept
    {
        return first.x * second.x + first.y * second.y;
    }

    [[nodiscard]] constexpr int Sign(float value) noexcept
    {
        return value < 0 ? -1 : value > 0 ? 1 : 0;
    }

    [[nodiscard]] constexpr int BiasedSign(float value) noexcept
    {
        return value < 0 ? -1 : 1;
    }

    [[nodiscard]] constexpr float ToDegrees(float radians) noexcept
    {
        return radians * 180 / std::numbers::pi_v<float>;
    }

    [[nodiscard]] constexpr float ToRadians(float degrees) noexcept
    {
        return degrees * std::numbers::pi_v<float> / 180;
    }
    [[nodiscard]] constexpr bool IsNormalized(float angle) noexcept
    {
        return angle >= -180 && angle <= 180;
    }

    [[nodiscard]] inline float NormalizeDegrees(float angle) noexcept
    {
        return std::fmodf(angle, 180) - static_cast<float>(180 * Sign(angle) * (static_cast<int>(angle) / 180 % 2));
    }

    [[nodiscard]] inline sf::Vector2f Lerp(sf::Vector2f start, sf::Vector2f end, float step)
    {
        sf::Vector2f to{ end - start };
        if (to == sf::Vector2f(0, 0))
            return start;
        float length{ Length(to) };
        return start + to / length * std::min(length, step);
    }

    [[nodiscard]] inline float LerpNormalizedAngle(float start, float end, float step) noexcept
    {
        assert(IsNormalized(start) && IsNormalized(end));
        float lineDistance{ std::fabs(end - start) };
        float distance{ std::min(lineDistance, 360 - lineDistance) };
        return start + step * Sign(end - start) * (distance == lineDistance ? 1 : -1);
    }

    [[nodiscard]] inline float DirectionToAngle(sf::Vector2f direction)
    {
        return ToDegrees(std::acosf(Dot(direction, { 0, -1 })) * BiasedSign(direction.x));
    }

    [[nodiscard]] inline sf::Vector2f AngleToDirection(float angle)
    {
        float radians{ ToRadians(angle) };
        return { std::sinf(radians), -std::cosf(radians) };
    }

    [[nodiscard]] inline bool ApproximativelyEqual(float first, float second) noexcept
    {
        return std::fabs(first - second) <= 0.00001f;
    }

    [[nodiscard]] inline bool ApproximativelyEqual(sf::Vector2f first, sf::Vector2f second) noexcept
    {
        return ApproximativelyEqual(first.x, second.x) && ApproximativelyEqual(first.y, second.y);
    }
}