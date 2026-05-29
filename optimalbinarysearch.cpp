#include <vector>
#include <cmath>

// Forward declaration of the condition function
bool condition(const std::vector<int>& arr, int idx);

// 1. Minimize x such that condition(x) is true
int binarySearchMin(const std::vector<int>& arr) {
    // lo points to an invalid value; hi can take all possible valid indices
    int lo = -1;
    int hi = static_cast<int>(arr.size());

    while (lo + 1 < hi) {
        int mid = lo + (hi - lo) / 2; // C++ integer division automatically truncates (like Math.floor)
        
        if (condition(arr, mid)) {
            hi = mid;
        } else {
            lo = mid;
        }
    }

    // In the minimization template, hi contains the return index
    return hi;
}

// 2. Maximize x such that condition(x) is true
int binarySearchMax(const std::vector<int>& arr) {
    // lo can take all possible valid indices; hi points to an invalid value
    int lo = -1;
    int hi = static_cast<int>(arr.size());

    while (lo + 1 < hi) {
        int mid = lo + (hi - lo) / 2;
        
        if (condition(arr, mid)) {
            lo = mid;
        } else {
            hi = mid;
        }
    }

    // In the maximization template, lo contains the return index
    return lo;
}

// Example condition function
bool condition(const std::vector<int>& arr, int idx) {
    // some condition on arr[idx]
    // return true or false
    return true;
}
