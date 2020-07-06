#include <string>
#include <cstring>
#include <iostream>
#include <utility>
#include <vector>

#define DT char
#define DT_ZERO 0
#define INTERNAL_BASE 10
#define INDEX_DT unsigned long
#define INDEX_DT_ONE 1ul
#define INDEX_DT_ZERO 0ul
#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)

class BigInteger {
private:
    std::vector<DT> digits;
    INDEX_DT size;
    bool isNegative;

    BigInteger(INDEX_DT size, std::vector<DT> digits) {
        this->size = size;
        this->digits = std::move(digits);
        this->digits.resize(size, DT_ZERO);

    }

    void shrink() {
        INDEX_DT i = this->digits.size() - 1;
        INDEX_DT j = i;
        for (; this->digits[j] == 0; j--);
        this->size = digits.size() - (i - (j + 1)) - 1;
        this->digits.resize(this->size);
        if (this->digits.empty()) this->isNegative = false;
    }

    int compareMagnitude(const BigInteger &another) const {
        if (size > another.size) return 1;
        else if (size < another.size) return -1;
        for (INDEX_DT i = size; i >= 0; i--) {
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

    static void absSum(const BigInteger &bigger, const BigInteger &smaller, BigInteger &result) {
        int carry = 0;
        for (INDEX_DT i = 0; i < smaller.size; i++) {
            int res = result.digits[i] + smaller.digits[i] + carry;
            carry = res / INTERNAL_BASE;
            res %= INTERNAL_BASE;
            result.digits[i] = res;
        }
        for (INDEX_DT i = smaller.size; carry != 0; i++) {
            result.digits[i] += carry;
            carry = result.digits[i] / INTERNAL_BASE;
            result.digits[i] %= INTERNAL_BASE;
        }
    }

    static void absDiff(const BigInteger &bigger, const BigInteger &smaller, BigInteger &result) {
        DT owe = 0;
        INDEX_DT i = 0;
        for (; i < smaller.size; i++) {
            result.digits[i] -= (smaller.digits[i] + owe);
            if (result.digits[i] >= 0) {
                owe = 0;
            } else {
                result.digits[i] += INTERNAL_BASE;
                owe = 1;
            }
        }
        if (i != bigger.size) {
            for (i = smaller.size; owe != 0 && i < bigger.size; i++) {
                result.digits[i] -= owe;
                if (result.digits[i] < 0) {
                    result.digits[i] += INTERNAL_BASE;
                    owe = 1;
                } else {
                    owe = 0;
                }
            }
        }
        result.digits[i] = abs(result.digits[i]);
    }

public:
    BigInteger(std::string num) {
        this->isNegative = num[0] == '-';
        this->size = num.length() - (this->isNegative ? 1 : 0);
        this->digits.resize(this->size);
        for (INDEX_DT i = 0; i < this->size; i++) this->digits[i] = num[num.length() - i - 1] - '0';
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
        BigInteger max = MAX(*this, other), min = MIN(*this, other);
        BigInteger maxByMagnitude = compareMagnitude(other) == 1 ? *this : other;
        BigInteger minByMagnitude = compareMagnitude(other) == -1 ? *this : other;
        BigInteger result(maxByMagnitude.size + 1, maxByMagnitude.digits);
        if (areSignsSame) {
            absSum(max, min, result);
            result.isNegative = this->isNegative;
        } else {
            absDiff(maxByMagnitude, minByMagnitude, result);
            result.isNegative = maxByMagnitude.isNegative;
        }
        result.shrink();
        return result;
    }

    std::string toString() {
        const int tmp = (this->isNegative ? 1 : 0);
        std::string result(this->size + tmp, ' ');
        if (this->isNegative) result[0] = '-';
        for (INDEX_DT i = tmp; i < this->size + tmp; i++) {
            result[i] = '0' + this->digits[this->size - 1 - (i - tmp)];
        }
        return result;
    }
};

int main() {
    std::string a, b;
    std::cin >> a >> b;
    BigInteger A(a), B(b);
    std::cout << A.toString() << " + " << B.toString() << " = " << (A + B).toString();
    return 0;
}