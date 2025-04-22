#include <numeric>
#include <iostream>

class Fraction {
    int64_t num;
    int64_t den;
public:
    Fraction(int64_t num = 0, int64_t den = 1) {
        if (den < 0) {
            num = -num;
            den = -den;
        }
        int64_t div = std::gcd(num, den);
        num /= div;
        den /= div;
        this->num = num;
        this->den = den;
    }

    bool isInt() const {
        return den == 1;
    }

    int64_t floor() const {
        return num / den; 
    }

    int64_t ceil() const {
        return (num + den - 1) / den;
    }

    int64_t getNum() const {
        return num;
    }

    int64_t getDen() const {
        return den;
    }

    float operator*() const {
        return (float)num / (float)den;
    }

    Fraction operator-() const {
        return Fraction(-num, den);
    }

    Fraction operator!() const {
        return Fraction(den, num);
    }

    Fraction operator+(Fraction other) const {
        return Fraction(num * other.den + den * other.num, den * other.den);
    }

    Fraction operator-(Fraction other) const {
        return (*this) + (-other);
    }

    Fraction operator*(Fraction other) const {
        return Fraction(num * other.num, den * other.den);
    }

    Fraction operator/(Fraction other) const {
        return (*this) * (!other);
    }

    bool operator==(Fraction other) const {
        return num == other.num && den == other.den;
    }

    bool operator!=(Fraction other) const {
        return !((*this) == other);
    }

    bool operator<(Fraction other) const {
        return num * other.den < other.num * den;
    }

    bool operator<=(Fraction other) const {
        return num * other.den <= other.num * den;
    }

    bool operator>(Fraction other) const {
        return num * other.den > other.num * den;
    }

    bool operator>=(Fraction other) const {
        return num * other.den >= other.num * den;
    }

    template<typename T>
    Fraction& operator+=(T value) {
        (*this) = (*this) + Fraction(value);
        return *this;
    }

    template<typename T>
    Fraction& operator-=(T value) {
        (*this) = (*this) - Fraction(value);
        return *this;
    }

    template<typename T>
    Fraction& operator*=(T value) {
        (*this) = (*this) * Fraction(value);
        return *this;
    }

    template<typename T>
    Fraction& operator/=(T value) {
        (*this) = (*this) / Fraction(value);
        return *this;
    }
};

template <typename T>
Fraction operator*(T value, Fraction frac) {
    return Fraction(value) * frac;
}

template <typename T>
Fraction operator*(Fraction frac, T value) {
    return frac * Fraction(value);
}

template <typename T>
Fraction operator/(T value, Fraction frac) {
    return Fraction(value) / frac;
}

template <typename T>
Fraction operator/(Fraction frac, T value) {
    return frac / Fraction(value);
}

template <typename T>
Fraction operator+(Fraction frac, T value) {
    return frac + Fraction(value);
}

template <typename T>
Fraction operator+(T value, Fraction frac) {
    return Fraction(value) + frac;
}

template <typename T>
Fraction operator-(T value, Fraction frac) {
    return Fraction(value) - frac;
}

template <typename T>
Fraction operator-(Fraction frac, T value) {
    return frac - Fraction(value);
}

template <typename T>
bool operator==(T value, Fraction frac) {
    return Fraction(value) == frac;
}

template <typename T>
bool operator==(Fraction frac, T value) {
    return frac == Fraction(value);
}

template <typename T>
bool operator!=(T value, Fraction frac) {
    return Fraction(value) != frac;
}

template <typename T>
bool operator!=(Fraction frac, T value) {
    return frac != Fraction(value);
}


template <typename T>
bool operator<(T value, Fraction frac) {
    return Fraction(value) < frac;
}

template <typename T>
bool operator<(Fraction frac, T value) {
    return frac < Fraction(value);
}

template <typename T>
bool operator<=(T value, Fraction frac) {
    return Fraction(value) <= frac;
}

template <typename T>
bool operator<=(Fraction frac, T value) {
    return frac <= Fraction(value);
}

template <typename T>
bool operator>(T value, Fraction frac) {
    return Fraction(value) > frac;
}

template <typename T>
bool operator>(Fraction frac, T value) {
    return frac > Fraction(value);
}

template <typename T>
bool operator>=(T value, Fraction frac) {
    return Fraction(value) >= frac;
}

template <typename T>
bool operator>=(Fraction frac, T value) {
    return frac >= Fraction(value);
}
