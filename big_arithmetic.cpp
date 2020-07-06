#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <stdio.h>

#define DT char
#define DT_ZERO (char)0
#define DT_ONE (char)1
#define INTERNAL_BASE (char)10
#define INDEX_DT unsigned long
#define INDEX_DT_ONE 1ul
#define INDEX_DT_ZERO 0ul
#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)
#define BIG_INTEGER_ZERO BigInteger("0")

class BigInteger {
private:
    std::vector<DT> digits;
    INDEX_DT size;
    bool isNegative{};

    BigInteger(INDEX_DT size, std::vector<DT> digits) {
        this->size = size;
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
        if (size > another.size) return 1;
        else if (size < another.size) return -1;
        for (INDEX_DT i = size; i != static_cast<INDEX_DT>(-INDEX_DT_ONE); i--) {
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
        for (INDEX_DT i = INDEX_DT_ZERO;
             i < std::max(result.digits.size(), smaller.digits.size()) || carry != 0; ++i) {
            if (i == result.digits.size()) result.digits.push_back(0);
            result.digits[i] += carry + (i < smaller.digits.size() ? smaller.digits[i] : INDEX_DT_ZERO);
            carry = result.digits[i] >= INTERNAL_BASE;
            if (carry != 0) result.digits[i] -= INTERNAL_BASE;
        }
    }

    static void absDiff(const BigInteger &smaller, BigInteger &result) {
        int carry = 0;
        for (INDEX_DT i = INDEX_DT_ZERO; i < smaller.digits.size() || carry != 0; ++i) {
            result.digits[i] -= carry + (i < smaller.digits.size() ? smaller.digits[i] : 0);
            carry = result.digits[i] < DT_ZERO;
            if (carry != 0) result.digits[i] += INTERNAL_BASE;
        }
    }

public:
    explicit BigInteger(std::string num) {
        this->isNegative = num[INDEX_DT_ZERO] == '-';
        this->size = num.length() - (this->isNegative ? INDEX_DT_ONE : INDEX_DT_ZERO);
        this->digits.resize(this->size);
        for (INDEX_DT i = INDEX_DT_ZERO; i < this->size; i++)
            this->digits[i] = num[num.length() - i - INDEX_DT_ONE] - '0';
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
        if (compareMagnitude(other) == 0 && !areSignsSame) return BIG_INTEGER_ZERO;
        BigInteger maxByMagnitude = compareMagnitude(other) == 1 ? *this : other;
        BigInteger minByMagnitude = compareMagnitude(other) == -1 ? *this : other;
        BigInteger result(maxByMagnitude.size, maxByMagnitude.digits);
//        if (areSignsSame) {
//            absSum(minByMagnitude, result);
//            result.isNegative = this->isNegative;
//        } else {
//            absDiff(minByMagnitude, result);
//            result.isNegative = maxByMagnitude.isNegative;
//        }
        absSum(minByMagnitude, result);
        result.shrink();
        return result;
    }

    BigInteger operator*(const BigInteger &other) const {
        if (*this == BIG_INTEGER_ZERO || other == BIG_INTEGER_ZERO) return BIG_INTEGER_ZERO;
        const INDEX_DT res_size = this->size + other.size;
        BigInteger result(res_size, std::vector<DT>(res_size, DT_ZERO));
        for (INDEX_DT i = INDEX_DT_ZERO; i < other.size; i++) {
            DT carry = DT_ZERO;
            for (INDEX_DT j = INDEX_DT_ZERO; j < this->size; j++) {
                result.digits[i + j] += carry + this->digits[j] * other.digits[i];
                carry = (DT) (result.digits[i + j] / INTERNAL_BASE);
                result.digits[i + j] %= INTERNAL_BASE;
            }
            result.digits[i + this->size] = carry;
        }
        result.isNegative = this->isNegative ^ other.isNegative;
        result.shrink();
        return result;
    }

    BigInteger operator-(const BigInteger &other) const {
        if (*this == other) return BIG_INTEGER_ZERO;
        bool areSignsSame = !(this->isNegative ^ other.isNegative);
        BigInteger maxByMagnitude = compareMagnitude(other) == 1 ? *this : other;
        BigInteger minByMagnitude = compareMagnitude(other) == -1 ? *this : other;
        BigInteger result(maxByMagnitude.size + INDEX_DT_ONE, maxByMagnitude.digits);
        if (areSignsSame) {
            absDiff(minByMagnitude, result);
            result.isNegative =
                    (*this == maxByMagnitude && this->isNegative) || (other == maxByMagnitude && !other.isNegative);
        } else {
            absSum(minByMagnitude, result);
            result.isNegative = (maxByMagnitude.isNegative && maxByMagnitude == *this) ||
                                (!maxByMagnitude.isNegative && maxByMagnitude == other);
        }
        result.shrink();
        return result;
    }

    std::string toString() const {
        const INDEX_DT tmp = (this->isNegative ? INDEX_DT_ONE : INDEX_DT_ZERO);
        std::string result(this->size + tmp, ' ');
        if (this->isNegative) result[INDEX_DT_ZERO] = '-';
        for (INDEX_DT i = tmp; i < this->size + tmp; i++) {
            result[i] = '0' + this->digits[this->size - INDEX_DT_ONE - (i - tmp)];
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
    BigInteger tmp = (A + B);
    std::cout << tmp.toString();
    fclose(stdout);
    fclose(stdin);
    return 0;
}