template <typename T>
class protectedMaxValue {
private:
    T value;
    T MaxValue;

public:
    protectedMaxValue(T initialAndMaxValue)
        : value(initialAndMaxValue), MaxValue(initialAndMaxValue) {}

    protectedMaxValue(T initialValue, T maxValue)
        : value(initialValue), MaxValue(maxValue) {}

    void setMaxValue(T max) {
        MaxValue = max;
        value = max;
    }

    T getMaxValue() const {
        return MaxValue;
    }

    protectedMaxValue& operator=(T newValue) {
        if(newValue <= 0){
            value = MaxValue;
        }
        else if (newValue > MaxValue) {
            value = MaxValue;
        }
        else{
            value = newValue;
        }
        return *this;
    }

    operator T() const {
        return value;
    }
};
