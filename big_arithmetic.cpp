#include <string>
#include <iostream>
#include <utility>
#include <vector>

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
        INDEX_DT i = this->digits.size() - INDEX_DT_ONE;
        INDEX_DT j = i;
        for (; this->digits[j] == INDEX_DT_ZERO && j != static_cast<INDEX_DT>(-INDEX_DT_ONE); j--);
        this->size = digits.size() - (i - (j + INDEX_DT_ONE)) - INDEX_DT_ONE;
        this->digits.resize(this->size);
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

    static void absSum(const BigInteger &bigger, const BigInteger &smaller, BigInteger &result) {
        DT carry = DT_ZERO;
        for (INDEX_DT i = INDEX_DT_ZERO; i < smaller.size; i++) {
            DT res = (DT) (result.digits[i] + smaller.digits[i] + carry);
            carry = (DT) (res / INTERNAL_BASE);
            res %= INTERNAL_BASE;
            result.digits[i] = res;
        }
        for (INDEX_DT i = smaller.size; carry != DT_ZERO; i++) {
            result.digits[i] += carry;
            carry = (DT) (result.digits[i] / INTERNAL_BASE);
            result.digits[i] %= INTERNAL_BASE;
        }
    }

    static void absDiff(const BigInteger &bigger, const BigInteger &smaller, BigInteger &result) {
        DT owe = DT_ZERO;
        INDEX_DT i = INDEX_DT_ZERO;
        for (; i < smaller.size; i++) {
            result.digits[i] -= (smaller.digits[i] + owe);
            if (result.digits[i] >= DT_ZERO) {
                owe = DT_ZERO;
            } else {
                result.digits[i] += INTERNAL_BASE;
                owe = DT_ONE;
            }
        }
        if (i != bigger.size) {
            for (i = smaller.size; owe != DT_ZERO && i < bigger.size; i++) {
                result.digits[i] -= owe;
                if (result.digits[i] < DT_ZERO) {
                    result.digits[i] += INTERNAL_BASE;
                    owe = DT_ONE;
                } else {
                    owe = DT_ZERO;
                }
            }
        }
        result.digits[i] = abs(result.digits[i]);
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
        BigInteger max = MAX(*this, other), min = MIN(*this, other);
        BigInteger maxByMagnitude = compareMagnitude(other) == 1 ? *this : other;
        BigInteger minByMagnitude = compareMagnitude(other) == -1 ? *this : other;
        BigInteger result(maxByMagnitude.size + INDEX_DT_ONE, maxByMagnitude.digits);
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
            absDiff(maxByMagnitude, minByMagnitude, result);
            result.isNegative =
                    (*this == maxByMagnitude && this->isNegative) || (other == maxByMagnitude && !other.isNegative);
        } else {
            absSum(maxByMagnitude, minByMagnitude, result);
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
    std::string a, b;
    std::cin >> a >> b;
    BigInteger A(a), B(b);
    BigInteger tmp = (A * B);
    std::cout << A.toString() << " + " << B.toString() << " = " << tmp.toString();
    return 0;
}