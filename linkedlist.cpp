#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

class LinkedList {
public:
    Node* head;
    LinkedList() : head(nullptr) {}

    void append(int val) {
        Node* newNode = new Node(val);
        if (!head) { head = newNode; return; }
        Node* curr = head;
        while (curr->next) curr = curr->next;
        curr->next = newNode;
    }

    void remove(int val) {
        if (!head) return;
        if (head->data == val) { Node* tmp = head; head = head->next; delete tmp; return; }
        Node* curr = head;
        while (curr->next && curr->next->data != val) curr = curr->next;
        if (curr->next) { Node* tmp = curr->next; curr->next = tmp->next; delete tmp; }
    }

    void print() {
        Node* curr = head;
        while (curr) { cout << curr->data << " -> "; curr = curr->next; }
        cout << "NULL\n";
    }

    ~LinkedList() {
        Node* curr = head;
        while (curr) { Node* tmp = curr; curr = curr->next; delete tmp; }
    }
};

int main() {
    LinkedList list;
    list.append(1);
    list.append(2);
    list.append(3);
    list.append(4);
    list.print();        // 1 -> 2 -> 3 -> 4 -> NULL

    list.remove(3);
    list.print();        // 1 -> 2 -> 4 -> NULL
    return 0;
}
