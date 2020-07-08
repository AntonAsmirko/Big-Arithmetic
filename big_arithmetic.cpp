#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdio>

#define DT int
#define INTERNAL_BASE 10

class BigInteger {
private:
    std::vector<DT> digits;
    bool isNegative;
    bool isNaN{};

    BigInteger(long long size, std::vector<DT> digits, bool isNegative) {
        this->digits = std::move(digits);
        this->digits.resize(size, 0);
        this->isNegative = isNegative;
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
        for (long long i = this->digits.size() - 1; i >= 0; i--) {
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
        for (long i = 0;
             i < std::max(result.digits.size(), smaller.digits.size()) || carry != 0; ++i) {
            if (i == result.digits.size()) result.digits.push_back(0);
            result.digits[i] += carry + (i < smaller.digits.size() ? smaller.digits[i] : 0);
            carry = result.digits[i] >= INTERNAL_BASE;
            if (carry != 0) result.digits[i] -= INTERNAL_BASE;
        }
    }

    static void absDiff(const BigInteger &smaller, BigInteger &result) {
        int carry = 0;
        for (long i = 0; i < smaller.digits.size() || carry != 0; ++i) {
            result.digits[i] -= carry + (i < smaller.digits.size() ? smaller.digits[i] : 0);
            carry = result.digits[i] < 0;
            if (carry != 0) result.digits[i] += INTERNAL_BASE;
        }
    }

    static bool isZero(const BigInteger &num) { return num.digits.size() == 1 && num.toString() == "0"; }

public:
    explicit BigInteger(std::string num) {
        this->isNegative = num[0] == '-';
        this->digits.resize(num.length() - (this->isNegative ? 1 : 0));
        for (long i = 0; i < this->digits.size(); i++)
            this->digits[i] = num[num.length() - i - 1] - '0';
    }

    bool isLowerZero() { return this->isNegative; }

    BigInteger(unsigned long long l) {
        this->isNegative = false;
        do {
            this->digits.push_back(l % INTERNAL_BASE);
            l /= INTERNAL_BASE;
        } while (l != 0);
    }

    std::vector<DT> getDigits() { return this->digits; }

    void makePositive() { this->isNegative = false; }

    void makeNegative() { this->isNegative = true; }

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
        BigInteger result(maxByMagnitude.digits.size() + 1, maxByMagnitude.digits, maxByMagnitude.isNegative);
        if (areSignsSame) absSum(minByMagnitude, result);
        else absDiff(minByMagnitude, result);
        result.shrink();
        return result;
    }

    BigInteger operator*(const BigInteger &other) const {
        if (isZero(*this) || isZero(other)) return BigInteger("0");
        const long long res_size = this->digits.size() + other.digits.size();
        BigInteger result(res_size, std::vector<DT>(res_size, (DT) 0), this->isNegative ^ other.isNegative);
        for (long i = 0; i < other.digits.size(); i++) {
            DT carry = 0;
            for (long j = 0; j < this->digits.size(); j++) {
                result.digits[i + j] += carry + this->digits[j] * other.digits[i];
                carry = (DT) (result.digits[i + j] / INTERNAL_BASE);
                result.digits[i + j] %= INTERNAL_BASE;
            }
            result.digits[i + this->digits.size()] = carry;
        }
        result.shrink();
        return result;
    }

    BigInteger operator-(const BigInteger &other) const {
        BigInteger tmp = other * BigInteger("-1");
        return *this + tmp;
    }

    void rightShift() {
        if (this->digits.empty()) {
            this->digits.push_back(0);
            return;
        }
        this->digits.push_back(this->digits[this->digits.size() - 1]);
        for (long long i = this->digits.size() - 2; i > 0; --i) this->digits[i] = this->digits[i - 1];
        this->digits[0] = 0;
    }

    BigInteger operator/(const BigInteger &right) {
        BigInteger b = right;
        b.isNegative = false;
        BigInteger result(0ull), current(0ull);
        result.digits.resize(this->digits.size());
        for (long long i = this->digits.size(); i >= 0; --i) {
            current.rightShift();
            current.digits[0] = this->digits[i];
            current.shrink();
            DT x = 0, l = 0, r = INTERNAL_BASE;
            while (l <= r) {
                DT m = (l + r) / 2;
                BigInteger t = b * m;
                if (t <= current) {
                    x = m;
                    l = m + 1;
                } else r = m - 1;
            }

            result.digits[i] = x;
            current = current - b * x;
        }

        result.isNegative = this->isNegative != right.isNegative;
        result.shrink();
        return result;
    }

    BigInteger sqrt() const {
        BigInteger zero(0), one(1), two(2);
        if (*this < zero) {/* handle NaN */}
        else if (*this <= one) return *this;
        BigInteger l(0), r = *this;
        while (r - l > one) {
            BigInteger i = (r - l) / two + l;
            if (i * i <= *this) l = i;
            else r = i;
        }
        return l;
    }

    BigInteger operator%(const BigInteger &other) {
        auto tmp = (*this / other);
        BigInteger result = *this - ((*this / other) * other);
        if (result.isNegative) result = result + other;
        return result;
    }

    std::string toString() const {
        const long tmp = (this->isNegative ? 1 : 0);
        std::string result(this->digits.size() + tmp, ' ');
        if (this->isNegative) result[0] = '-';
        for (long i = tmp; i < this->digits.size() + tmp; i++) {
            result[i] = '0' + this->digits[this->digits.size() - 1 - (i - tmp)];
        }
        return result;
    }

    ~BigInteger() {
        int a = 0;
    }

};

int main() {
    freopen("INPUT.TXT", "r", stdin);
    freopen("OUTPUT.TXT", "w", stdout);
    std::string a;
    std::cin >> a;
    BigInteger A(a);
    auto tmp = A.sqrt();
    std::cout << tmp.toString();
    fclose(stdout);
    fclose(stdin);
    return 0;
}