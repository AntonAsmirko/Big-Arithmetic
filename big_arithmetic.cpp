#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdio>

#define DT char
#define DT_ZERO (char)0
#define INTERNAL_BASE (char)10

class BigInteger {
private:
    std::vector<DT> digits;
    bool isNegative{};
    bool isNaN{};

    BigInteger(size_t size, std::vector<DT> digits) {
        this->digits = std::move(digits);
        this->digits.resize(size, DT_ZERO);

    }

    void shrink() {
        while (this->digits.size() > 1 && this->digits.back() == 0) {
            this->digits.pop_back();
        }
        if (this->digits.size() == 1 && this->digits[0] == 0) this->isNegative = false;
    }

    int compareMagnitude(const BigInteger &another) const {
        if (this->digits.size() > another.digits.size()) return 1;
        else if (this->digits.size() < another.digits.size()) return -1;
        for (size_t i = this->digits.size(); i != static_cast<size_t>(-1); i--) {
            if (this->digits[i] > another.digits[i]) return 1;
            else if (this->digits[i] < another.digits[i]) return -1;
        }
        return 0;
    }

    int compare(const BigInteger &another) const {
        if (this->isNegative && !another.isNegative) return -1;
        if (!this->isNegative && another.isNegative) return 1;
        int compare_res = compareMagnitude(another);
        if (compare_res == 1) {
            if (this->isNegative && another.isNegative) return -1;
            else if (!this->isNegative && !another.isNegative) return 1;
        } else if (compare_res == -1) {
            if (this->isNegative && another.isNegative) return 1;
            else if (!this->isNegative && !another.isNegative) return -1;
        }
        return 0;
    }

    static void absSum(const BigInteger &smaller, BigInteger &result) {
        int carry = 0;
        for (size_t i = 0;
             i < std::max(result.digits.size(), smaller.digits.size()) || carry != 0; ++i) {
            if (i == result.digits.size()) result.digits.push_back(0);
            result.digits[i] += carry + (i < smaller.digits.size() ? smaller.digits[i] : 0);
            carry = result.digits[i] >= INTERNAL_BASE;
            if (carry != 0) result.digits[i] -= INTERNAL_BASE;
        }
    }

    static void absDiff(const BigInteger &smaller, BigInteger &result) {
        int carry = 0;
        for (size_t i = 0; i < smaller.digits.size() || carry != 0; ++i) {
            result.digits[i] -= carry + (i < smaller.digits.size() ? smaller.digits[i] : 0);
            carry = result.digits[i] < DT_ZERO;
            if (carry != 0) result.digits[i] += INTERNAL_BASE;
        }
    }

    static bool isZero(const BigInteger &num) { return num.digits.size() == 1 && num.toString() == "0"; }

public:
    explicit BigInteger(std::string num) {
        this->isNegative = num[0] == '-';
        this->digits.resize(num.length() - (this->isNegative ? 1 : 0));
        for (size_t i = 0; i < this->digits.size(); i++)
            this->digits[i] = num[num.length() - i - 1] - '0';
    }

    bool operator==(const BigInteger &other) const { return compare(other) == 0; }

    bool operator!=(const BigInteger &other) const { return compare(other) != 0; }

    bool operator>(const BigInteger &other) const { return compare(other) == 1; }

    bool operator<(const BigInteger &other) const { return compare(other) == -1; }

    bool operator>=(const BigInteger &other) const {
        int compare_result = compare(other);
        return compare_result == 1 || compare_result == 0;
    }

    bool operator<=(const BigInteger &other) const {
        int compare_result = compare(other);
        return compare_result == -1 || compare_result == 0;
    }

    BigInteger operator+(const BigInteger &other) const {
        bool areSignsSame = !(this->isNegative ^ other.isNegative);
        if (compareMagnitude(other) == 0 && !areSignsSame) return BigInteger("0");
        BigInteger maxByMagnitude = compareMagnitude(other) == 1 ? *this : other;
        BigInteger minByMagnitude = compareMagnitude(other) == -1 ? *this : other;
        BigInteger result(maxByMagnitude.digits.size() + 1, maxByMagnitude.digits);
        if (areSignsSame) {
            absSum(minByMagnitude, result);
            result.isNegative = this->isNegative;
        } else {
            absDiff(minByMagnitude, result);
            result.isNegative = maxByMagnitude.isNegative;
        }
        result.shrink();
        return result;
    }

    BigInteger operator*(const BigInteger &other) const {
        if (isZero(*this) || isZero(other)) return BigInteger("0");
        const size_t res_size = this->digits.size() + other.digits.size();
        BigInteger result(res_size, std::vector<DT>(res_size, DT_ZERO));
        for (size_t i = 0; i < other.digits.size(); i++) {
            DT carry = DT_ZERO;
            for (size_t j = 0; j < this->digits.size(); j++) {
                result.digits[i + j] += carry + this->digits[j] * other.digits[i];
                carry = (DT) (result.digits[i + j] / INTERNAL_BASE);
                result.digits[i + j] %= INTERNAL_BASE;
            }
            result.digits[i + this->digits.size()] = carry;
        }
        result.isNegative = this->isNegative ^ other.isNegative;
        result.shrink();
        return result;
    }

    BigInteger operator-(const BigInteger &other) const {
        BigInteger tmp = other * BigInteger("-1");
        return *this + tmp;
    }

    BigInteger rightShift() const {
        BigInteger result(this->digits.size(), this->digits);
        result.digits.push_back(this->digits[this->digits.size() - 1]);
        for (size_t i = result.digits.size() - 2; i > 0; --i) result.digits[i] = result.digits[i - 1];
        result.digits[0] = 0;
        return result;
    }

    BigInteger operator/(const BigInteger &other) const {
        if (isZero(other)) {

        }
        BigInteger tmp = other;
        tmp.isNegative = false;
        BigInteger result(this->digits.size(), std::vector<DT>(this->digits.size(), 0));
        BigInteger current("0");
        for (long long i = static_cast<long long>(result.digits.size()) - 1; i >= 0; --i) {
            current = current.rightShift();
            current.digits[0] = this->digits[i];
            current.shrink();
            long long x = 0, l = 0, r = INTERNAL_BASE;
            while (l <= r) {
                long long m = (l + r) / 2;
                BigInteger t = tmp * BigInteger(std::to_string(m));
                if (t <= current) {
                    x = m;
                    l = m + 1;
                } else r = m - 1;
            }

            result.digits[i] = x;
            current = current - tmp * BigInteger(std::to_string(x));
        }

        result.isNegative = this->isNegative != other.isNegative;
        result.shrink();
        return result;
    }

    BigInteger operator%(const BigInteger &other) {
        BigInteger result = *this - *this / other * other;
        if (result.isNegative) result = result + other;
        return result;
    }

    BigInteger sqrt() {
        BigInteger one = BigInteger("1");
        if (isZero(*this) || *this == one) return *this;
        BigInteger result = one, i = one;
        while (result <= *this) {
            i = i + one;
            result = i * i;
        }
        return i - one;
    }

    std::string toString() const {
        const size_t tmp = (this->isNegative ? 1 : 0);
        std::string result(this->digits.size() + tmp, ' ');
        if (this->isNegative) result[0] = '-';
        for (size_t i = tmp; i < this->digits.size() + tmp; i++) {
            result[i] = '0' + this->digits[this->digits.size() - 1 - (i - tmp)];
        }
        return result;
    }
};

int main() {
    freopen("INPUT.TXT", "r", stdin);
    freopen("OUTPUT.TXT", "w", stdout);
    std::string a, b;
    std::cin >> a >> b;
    BigInteger A(a), B(b);
    BigInteger tmp = A / B;
    std::cout << tmp.toString();
    fclose(stdout);
    fclose(stdin);
    return 0;
}