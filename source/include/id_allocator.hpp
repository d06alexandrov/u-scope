#pragma once

#include <concepts>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>

namespace UData {

/**
 * @brief An empty container that does not contain any elements.
 *
 * This is used as a default container type for the get_available_id function when no allocated IDs
 * are provided.
 */
struct EmptyContainer
{
    /**
     * @brief Check if the container contains a given element.
     *
     * @tparam T The type of the element to check for.
     *
     * @param key The element to check for.
     * @return false, as this container does not contain any elements.
     */
    template <typename T>
    [[nodiscard]] constexpr bool contains(T key) const noexcept
    {
        return false;
    }
};

/**
 * @brief Get an available ID of type IdType that is not already in use.
 *
 * @tparam IdType The type of the ID to be generated.
 * @tparam MaxCapacity The maximum number of IDs that can be generated.
 * @tparam Container The type of the container that holds the allocated IDs.
 *
 * @param allocated_ids The container that holds the allocated IDs.
 * @return An available ID of type IdType that is not already in use.
 */
template <typename IdType,
          size_t MaxCapacity = static_cast<size_t>(std::numeric_limits<IdType>::max()) - 1,
          typename Container = EmptyContainer>
    requires requires(const Container &c, IdType id) {
        { c.contains(id) } -> std::convertible_to<bool>;
    }
[[nodiscard]] IdType get_available_id(const Container &allocated_ids = Container{ })
{
    for (size_t i = 0; i < MaxCapacity; i++) {
        const auto potential_id = static_cast<IdType>(i);

        if (!allocated_ids.contains(potential_id)) {
            return potential_id;
        }
    }

    throw std::range_error("ID capacity limit exceeded: " + std::to_string(MaxCapacity));
}

} // namespace UData
