#pragma once

template <typename T, int N>
class CircularBuffer {
private:
    T buffer[N+1];
    int head = 0;
    int tail = 0;
    bool headIsReset = false;

public:

    CircularBuffer() : head(0), tail(0) {}

//******************************************************
// Interupt side
//******************************************************

    void push(const T& item) {
        buffer[tail] = item;
        tail = (tail + 1) % (N+1);
    }

    void resetTail(){
        tail = 0;
        headIsReset = true;
    }

    int getAvailableSpace() const {
        if(headIsReset){
            return N - head;
        }
        else if (tail >= head) {
            return N - (tail - head);
        } else {
            return head - tail;
        }
    }

    int getUsedSpace() const {
        return N - getAvailableSpace();
    }

    bool isFull() const {
        return getAvailableSpace() == 0;
    }

//******************************************************
// loop side
//******************************************************

    T pop() {
        T item = buffer[head];
        head = (head + 1) % (N+1);
        return item;
    }

    T* read(int i) {
        int localHead = (head + i) % (N+1);
        return &(buffer[localHead]);
    }

    void resetHead(){
        head = 0;
        headIsReset = false;
    }

//******************************************************
// All side
//******************************************************

    //not protected
    bool isEmpty(int i = 0) const {
        int localHead = (head + i) % (N+1);
        if(headIsReset){
            return true;
        }
        else{
            return (localHead == tail);
        }
    }

    int getCapacity() const {
        return N;
    }
};