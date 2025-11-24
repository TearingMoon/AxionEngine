#include <vector>
#include <algorithm>

class PtrVectorFilter
{
public:
    template <typename T, typename Pred>
    std::vector<T *> filter_ptr_vector(const std::vector<T *> &input, Pred pred)
    {
        std::vector<T *> result;
        result.reserve(input.size());

        std::copy_if(input.begin(), input.end(), std::back_inserter(result),
                     [&](T *ptr)
                     {
                         return ptr != nullptr && pred(*ptr);
                     });

        return result;
    }
};