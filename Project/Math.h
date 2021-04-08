#pragma once
#include <cassert>
#include <cmath>
#include <numbers>
#include <array>
#include <algorithm>
#include <concepts>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Transformable.hpp>

namespace Math
{
    [[nodiscard]] inline float Length(sf::Vector2f vector)
    {
        return sqrtf(powf(vector.x, 2) + powf(vector.y, 2));
    }

    [[nodiscard]] inline sf::Vector2f Normalize(sf::Vector2f vector)
    {
        float magnitude = Length(vector);
        if (magnitude == 0)
            return vector;
        return vector / magnitude;
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

    template<typename Number> requires std::floating_point<Number> || std::signed_integral<Number>
    [[nodiscard]] inline Number ConstantIncrement(Number start, Number end, Number step) noexcept
    {
        if (start == end)
            return start;
        if (start < end)
            return std::min(start + step, end);
        return std::max(start + step, end);
    }

    [[nodiscard]] inline sf::Vector2f ConstantIncrement(sf::Vector2f start, sf::Vector2f end, float step)
    {
        sf::Vector2f to{ end - start };
        if (to == sf::Vector2f(0, 0))
            return start;
        float length{ Length(to) };
        return start + to / length * std::min(length, step);
    }

    [[nodiscard]] inline float ConstantIncrementNormalizedAngle(float start, float end, float step) noexcept
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

    [[nodiscard]] inline sf::Vector2f Confine(sf::Vector2f position, const sf::FloatRect& area)
    {
        return { std::clamp(position.x, area.left, area.left + area.width), 
            std::clamp(position.y, area.top, area.top + area.height) };
    }

    inline void LocalRectangleToGlobal(const sf::FloatRect& from, const sf::Transformable& convertor, std::array<sf::Vector2f, 4>& to)
    {
        to[0] = { from.left, from.top };
        to[1] = { from.left + from.width, from.top };
        to[2] = { from.left + from.width, from.top + from.height };
        to[3] = { from.left, from.top + from.height };
        std::transform(to.begin(), to.end(), to.begin(), [&convertor](sf::Vector2f localRectanglePoint)
            {
                return convertor.getTransform().transformPoint(localRectanglePoint);
            });
    }

    [[nodiscard]] inline bool AreRectanglesIntersecting(const std::array<sf::Vector2f, 4>& first, const std::array<sf::Vector2f, 4>& second)
    {
        std::array<sf::Vector2f, 4> axis
        {
            first[1] - first[0],
            first[2] - first[1],
            second[1] - second[0],
            second[2] - second[1]
        };
        std::array<float, 4> lineProjectedPoints{};
        for (sf::Vector2f singleAxis : axis)
        {
            std::transform(first.begin(), first.end(), lineProjectedPoints.begin(), [singleAxis](sf::Vector2f point)
                {
                    return Dot(point, singleAxis);
                });
            auto lineProjectedPointsRange = std::minmax_element(lineProjectedPoints.begin(), lineProjectedPoints.end());
            std::pair<float, float> firstLineProjectedPointsRange{ *lineProjectedPointsRange.first, *lineProjectedPointsRange.second };
            std::transform(second.begin(), second.end(), lineProjectedPoints.begin(), [singleAxis](sf::Vector2f point)
                {
                    return Dot(point, singleAxis);
                });
            lineProjectedPointsRange = std::minmax_element(lineProjectedPoints.begin(), lineProjectedPoints.end());
            std::pair<float, float> secondLineProjectedPointsRange{ *lineProjectedPointsRange.first, *lineProjectedPointsRange.second };
            if (firstLineProjectedPointsRange.second < secondLineProjectedPointsRange.first ||
                secondLineProjectedPointsRange.second < firstLineProjectedPointsRange.first)
                return false;
        }
        return true;
    }
}