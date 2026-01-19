#pragma once
#include <vector>
#include <algorithm>

namespace Axion
{

/**
 * @brief Filter a vector of pointers by predicate.
 * @tparam T Element type.
 * @tparam Pred Predicate type (takes T& and returns bool).
 * @param input Vector of pointers to filter.
 * @param pred Predicate function.
 * @return New vector containing non-null pointers matching the predicate.
 */
template <typename T, typename Pred>
inline std::vector<T*> filter_ptr_vector(const std::vector<T*>& input, Pred pred)
{
    std::vector<T*> result;
    result.reserve(input.size());

    std::copy_if(input.begin(), input.end(), std::back_inserter(result),
                 [&](T* ptr)
                 {
                     return ptr != nullptr && pred(*ptr);
                 });

    return result;
}

} // namespace Axion