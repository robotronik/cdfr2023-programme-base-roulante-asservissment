#include <functional>

template <typename T>
class protectedMaxValue {
private:
    T MinValue;
    T MaxValue;
    T Value;
    std::function<void(T)> onChange1;

public:

    protectedMaxValue(T minValue, T maxValue, T value = 0)
        : MinValue(minValue), MaxValue(maxValue), Value(value) {}

    void setRange(T minValue, T maxValue) {
        MinValue = minValue;
        MaxValue = maxValue;
        if(Value < MinValue){
            Value = MinValue;
            if (onChange1)onChange1(Value);
        }
        else if (Value > MaxValue) {
            Value = MaxValue;
            if (onChange1)onChange1(Value);
        }
    }

    void setOnChangeCallback(std::function<void(T)> callback) {
        onChange1 = callback;
    }

    T getMaxValue() const {
        return MaxValue;
    }

    T getMinValue() const {
        return MinValue;
    }

    void setMin(){
        Value = MinValue;
        if (onChange1)onChange1(Value);
    }

    void setMax(){
        Value = MaxValue;
        if (onChange1)onChange1(Value);
    }

    protectedMaxValue& operator=(T newValue) {
        if(newValue < MinValue){
            Value = MinValue;
        }
        else if (newValue > MaxValue) {
            Value = MaxValue;
        }
        else{
            Value = newValue;
        }
        if (onChange1)onChange1(Value);
        return *this;
    }

    operator T() const {
        return Value;
    }
};
