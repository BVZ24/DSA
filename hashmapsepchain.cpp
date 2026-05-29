#include <iostream>
#include <vector>
#include <list>
#include <optional>
using namespace std;

class HashMap {
    vector<list<pair<int,int>>> table;
    int capacity;
    int size;

    int hash(int key) {
        return key % capacity;
    }

public:
    HashMap(int cap = 16) : capacity(cap), size(0), table(cap) {}

    void insert(int key, int value) {
        int idx = hash(key);
        for (auto& [k, v] : table[idx]) {
            if (k == key) { v = value; return; }  // update existing
        }
        table[idx].emplace_back(key, value);
        size++;
        if ((float)size / capacity >= 0.75f) resize();
    }

    optional<int> get(int key) {
        int idx = hash(key);
        for (auto& [k, v] : table[idx])
            if (k == key) return v;
        return nullopt;
    }

    void remove(int key) {
        int idx = hash(key);
        auto& chain = table[idx];
        chain.remove_if([&](auto& pair) {
            if (pair.first == key) { size--; return true; }
            return false;
        });
    }

    void resize() {
        vector<list<pair<int,int>>> old = table;
        capacity *= 2;
        table.assign(capacity, {});
        size = 0;
        for (auto& chain : old)
            for (auto& [k, v] : chain)
                insert(k, v);
    }

    void print() {
        for (int i = 0; i < capacity; i++) {
            if (table[i].empty()) continue;
            cout << "[" << i << "] ";
            for (auto& [k, v] : table[i])
                cout << k << ":" << v << " -> ";
            cout << "\n";
        }
    }
};

int main() {
    HashMap map;
    map.insert(1,  100);
    map.insert(17, 200);   // 17 % 16 == 1 — collides, chains at bucket 1
    map.insert(33, 300);   // 33 % 16 == 1 — collides again
    map.insert(5,  500);

    map.print();
    // [1] 1:100 -> 17:200 -> 33:300 ->
    // [5] 5:500 ->

    auto v = map.get(17);
    if (v) cout << "key 17 -> " << *v << "\n";   // 200

    map.remove(17);
    map.print();
    // [1] 1:100 -> 33:300 ->
}
