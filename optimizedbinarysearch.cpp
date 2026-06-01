// Invariant:
//     arr[left] < x <= arr[right]
// Return:
//     Lowest 'right' satisfying the condition
template <typename T, size_t Size>
[[nodiscard]] size_t lower_bound(std::span<T, Size> arr, T x) {
    // Assuming: arr[left]  = -infinity
    // Assuming: arr[right] = +infinity
    int left  = -1;
    int right = static_cast<int>(Size);

    while (right - left > 1) {
        int mid = left + (right - left) / 2;
        if (arr[mid] < x) {
            left = mid;
        } else {
            right = mid;
        }
    }
    return static_cast<size_t>(right);
}
