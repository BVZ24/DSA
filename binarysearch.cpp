#include <iostream>
#include <string>
using namespace std;

int binSearch(string str[], int n, string target) {
    int left = 0, right = n - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (str[mid] == target)
            return mid;
        else if (str[mid] < target)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}

int main() {
    string str[] = {"apple", "banana", "cherry", "grape", "orange"};
    int n = sizeof(str) / sizeof(str[0]);
    string target = "grape";

    cout << "Given array: ";
    for (int i = 0; i < n; i++) {
        cout << str[i] << " ";
    }
    cout << endl;

    cout << "Target String to search: " << target << endl;

    int result = binSearch(str, n, target);
    cout << "Element found at index: " << result << endl;

    return 0;
}
