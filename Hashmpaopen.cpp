#include <iostream>
#include <vector>
#include <string>
using namespace std;

enum State { EMPTY, OCCUPIED, DELETED };

struct Entry {
    int key;
    int value;
    State state = EMPTY;
};

class HashMap {
    vector<Entry> table;
    int capacity;
    int size;

    int hash(int key) {
        return key % capacity;
    }

public:
    HashMap(int cap = 16) : capacity(cap), size(0), table(cap) {}

    void insert(int key, int value) {
        if ((float)size / capacity >= 0.7f) resize();
        int idx = hash(key);
        while (table[idx].state == OCCUPIED && table[idx].key != key)
            idx = (idx + 1) % capacity;   // linear probe
        if (table[idx].state != OCCUPIED) size++;
        table[idx] = {key, value, OCCUPIED};
    }

    int* get(int key) {
        int idx = hash(key);
        int probes = 0;
        while (table[idx].state != EMPTY && probes < capacity) {
            if (table[idx].state == OCCUPIED && table[idx].key == key)
                return &table[idx].value;
            idx = (idx + 1) % capacity;
            probes++;
        }
        return nullptr;   // not found
    }

    void remove(int key) {
        int idx = hash(key);
        int probes = 0;
        while (table[idx].state != EMPTY && probes < capacity) {
            if (table[idx].state == OCCUPIED && table[idx].key == key) {
                table[idx].state = DELETED;   // tombstone
                size--;
                return;
            }
            idx = (idx + 1) % capacity;
            probes++;
        }
    }

    void resize() {
        vector<Entry> old = table;
        capacity *= 2;
        table.assign(capacity, Entry{});
        size = 0;
        for (auto& e : old)
            if (e.state == OCCUPIED) insert(e.key, e.value);
    }
};

int main() {
    HashMap map;
    map.insert(1,  100);
    map.insert(17, 200);   // 17 % 16 == 1 — collides with key 1
    map.insert(33, 300);   // 33 % 16 == 1 — collides again

    int* v = map.get(17);
    if (v) cout << "key 17 -> " << *v << "\n";   // 200

    map.remove(17);
    v = map.get(17);
    cout << (v ? "found" : "not found") << "\n";  // not found
    return 0;
}
