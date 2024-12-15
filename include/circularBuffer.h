#pragma once

template <typename T, int N>
class CircularBuffer {
private:
    T buffer[N];
    int head = 0;
    int tail = 0;
    bool full = false;

public:

    CircularBuffer() : head(0), tail(0), full(false) {}

    void push(const T& item) {
        if (full) {
        }

        buffer[tail] = item;
        tail = (tail + 1) % N;
        full = (tail == head);
    }

    T pop() {
        if (isEmpty()) {
        }

        T item = buffer[head];
        head = (head + 1) % N;
        full = false;
        return item;
    }

    bool isEmpty() const {
        return (!full && (head == tail));
    }

    bool isFull() const {
        return full;
    }

    int getAvailableSpace() const {
        if (full) {
            return 0;
        }
        if (tail >= head) {
            return N - (tail - head);
        } else {
            return head - tail;
        }
    }

    int getUsedSpace() const {
        if (full) {
            return N;
        }
        if (tail >= head) {
            return tail - head;
        } else {
            return N - (head - tail);
        }
    }

    int getCapacity() const {
        return N;
    }
};