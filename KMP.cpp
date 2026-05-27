#include <iostream>
#include <string>
#include <vector>
using namespace std;

void computeLPS(string pattern, vector<int>& lps) {
    int m = pattern.length();
    int length = 0;
    lps[0] = 0;
    int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[length]) {
            length++;
            lps[i] = length;
            i++;
        } else {
            if (length != 0) {
                length = lps[length - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

void KMPSearch(string text, string pattern) {
    int n = text.length();
    int m = pattern.length();

    vector<int> lps(m);
    computeLPS(pattern, lps);

    int i = 0; // index for text
    int j = 0; // index for pattern
    int count = 0;

    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }

        if (j == m) {
            cout << "Pattern found at index: " << i - j << endl;
            count++;
            j = lps[j - 1];
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    if (count == 0) {
        cout << "No match found!" << endl;
    } else {
        cout << "Total matches found: " << count << endl;
    }
}

int main() {
    string text = "xyztrwqxyzfg";
    string pattern = "xyz";

    cout << "Text: " << text << endl;
    cout << "Pattern: " << pattern << endl;
    cout << "Search results:" << endl;

    KMPSearch(text, pattern);

    return 0;
}
