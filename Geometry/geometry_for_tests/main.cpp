#include <iostream>

class Vector {
public:
    Vector() {
        x_ = 0;
        y_ = 0;
    }
    Vector(int64_t x, int64_t y) : x_(x), y_(y) {}
    int64_t operator*(const Vector& other) {
        return x_ * other.x_ + y_ * other.y_;
    }
    int64_t operator^(const Vector& other) {
        return x_ * other.y_ - y_ * other.x_;
    }
    Vector& operator+=(const Vector& other) {
        x_ += other.x_;
        y_ += other.y_;
        return *this;
    }
    Vector operator+(const Vector& other) {
        Vector copy = *this;
        copy += other;
        return copy;
    }
    Vector& operator-=(const Vector& other) {
        x_ -= other.x_;
        y_ -= other.y_;
        return *this;
    }
    Vector operator-(const Vector& other) {
        Vector copy = *this;
        copy -= other;
        return copy;
    }
    Vector& operator*=(const int64_t& value) {
        x_ *= value;
        y_ *= value;
        return *this;
    }
    Vector operator*(const int64_t& value) {
        Vector copy = *this;
        copy *= value;
        return copy;
    }
    /*Vector operator*(const Vector& other, const int64_t& value) {
        Vector copy = other;
        other *= value;
        return copy;
    }*/
    Vector operator-(){
        Vector copy;
        copy.x_ = -copy.x_;
        copy.y_ = -copy.y_;
        return copy;
    }
    int64_t GetX() const {
        return x_;
    }
    int64_t GetY() const {
        return y_;
    }
private:
    int64_t x_;
    int64_t y_;
};




int main(int argc, const char * argv[]) {
    
    return 0;
}
