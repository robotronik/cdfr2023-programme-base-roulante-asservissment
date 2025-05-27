#pragma once

template <typename T, int N>
class CircularBuffer {
private:
    T buffer[N+1];
    int tail = 0;
    int head = 0;
    bool headIsReset = false;

public:

    CircularBuffer() : tail(0), head(0) {}

//******************************************************
// Interupt side
//******************************************************

    void push(const T& item) {
        buffer[head] = item;
        head = (head + 1) % (N+1);
    }

    void resetHead(){
        head = 0;
        headIsReset = true;
    }

    int getAvailableSpace() const {
        if(headIsReset){
            return N - tail;
        }
        else if (head >= tail) {
            return tail - head;
        } else {
            return N - (head - tail);
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
        T item = buffer[tail];
        tail = (tail + 1) % (N+1);
        return item;
    }

    void resetTail(){
        tail = 0;
        headIsReset = false;
    }

//******************************************************
// All side
//******************************************************

    //not protected
    bool isEmpty() const {
        if(headIsReset){
            return true;
        }
        else{
            return (tail == head);
        }
    }

    int getCapacity() const {
        return N;
    }
};