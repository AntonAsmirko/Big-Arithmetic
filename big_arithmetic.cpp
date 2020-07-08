#include <string>
#include <vector>
#include <exception>
#include <cstdio>
#include <climits>
#include <cstring>

#define DT int
#define INTERNAL_BASE 10

class NaN_ERROR : public std::exception {
    const char *what() const noexcept override { return "NaN"; }
};

class DivisionByZeroError : public std::exception {
    const char *what() const noexcept override { return "division by zero"; }
};

class SquareRootOfNegativeNumberError : public std::exception {
    const char *what() const noexcept override { return "square root of negative number"; }
};

class BigInteger {
private:
    std::vector<DT> digits;
    bool isNegative;
    bool isNaN = false;

    BigInteger(long long size, std::vector<DT> digits, bool isNegative) {
        this->digits = std::move(digits);
        this->digits.resize(size, 0);
        this->isNegative = isNegative;
    }

    void shrink() {
        while (this->digits.size() > 1 && this->digits.back() == 0) this->digits.pop_back();
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
        if (another.isNaN || this->isNaN) throw NaN_ERROR();
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
        for (long long i = 0;
             i < std::max(result.digits.size(), smaller.digits.size()) || carry != 0; ++i) {
            if (i == result.digits.size()) result.digits.push_back(0);
            result.digits[i] += carry + (i < smaller.digits.size() ? smaller.digits[i] : 0);
            carry = result.digits[i] >= INTERNAL_BASE;
            if (carry != 0) result.digits[i] -= INTERNAL_BASE;
        }
    }

    static void absDiff(const BigInteger &smaller, BigInteger &result) {
        int carry = 0;
        for (long long i = 0; i < smaller.digits.size() || carry != 0; ++i) {
            result.digits[i] -= carry + (i < smaller.digits.size() ? smaller.digits[i] : 0);
            carry = result.digits[i] < 0;
            if (carry != 0) result.digits[i] += INTERNAL_BASE;
        }
    }

    static bool isZero(const BigInteger &num) { return num.digits.size() == 1 && num.toString() == "0"; }

public:
    explicit BigInteger(std::string num) {
        if (num == "NaN") {
            this->isNaN = true;
        } else {
            this->isNegative = num[0] == '-';
            this->digits.resize(num.length() - (this->isNegative ? 1 : 0));
            for (long i = 0; i < this->digits.size(); i++) this->digits[i] = num[num.length() - i - 1] - '0';
        }
    }

    explicit BigInteger(signed long long l) {
        if (l < 0) {
            this->isNegative = true;
            l = -l;
        } else this->isNegative = false;
        do {
            this->digits.push_back(l % INTERNAL_BASE);
            l /= INTERNAL_BASE;
        } while (l != 0);
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
        if (this->isNaN || other.isNaN) throw NaN_ERROR();
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
        if (this->isNaN || other.isNaN) throw NaN_ERROR();
        if (isZero(*this) || isZero(other)) return BigInteger("0");
        const long long res_size = this->digits.size() + other.digits.size();
        BigInteger result(res_size, std::vector<DT>(res_size, (DT) 0), this->isNegative ^ other.isNegative);
        for (long long i = 0; i < other.digits.size(); i++) {
            DT carry = 0;
            for (long long j = 0; j < this->digits.size(); j++) {
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
        if (this->isNaN || other.isNaN) throw NaN_ERROR();
        return *this + other * BigInteger("-1");
    }

    void rightShift() {
        this->digits.push_back(this->digits[this->digits.size() - 1]);
        for (long long i = this->digits.size() - 2; i > 0; --i) this->digits[i] = this->digits[i - 1];
        this->digits[0] = 0;
    }

    BigInteger operator/(const BigInteger &other) {
        if (this->isNaN || other.isNaN) throw NaN_ERROR();
        if (isZero(other)) throw DivisionByZeroError();
        BigInteger otherAbs = other;
        otherAbs.isNegative = false;
        BigInteger result(0), tmp(0);
        result.digits.resize(this->digits.size());
        for (long long i = this->digits.size() - 1; i >= 0; --i) {
            tmp.rightShift();
            tmp.digits[0] = this->digits[i];
            tmp.shrink();
            DT x = 0, l = 0, r = INTERNAL_BASE;
            while (l <= r) {
                DT m = (l + r) / 2;
                BigInteger t = otherAbs * BigInteger(m);
                if (t <= tmp) {
                    x = m;
                    l = m + 1;
                } else r = m - 1;
            }
            result.digits[i] = x;
            tmp = tmp - otherAbs * BigInteger(x);
        }
        result.isNegative = this->isNegative != other.isNegative;
        result.shrink();
        return result;
    }

    BigInteger sqrt() const {
        if (this->isNaN) throw NaN_ERROR();
        BigInteger zero(0), one(1), two(2);
        if (*this < zero) throw SquareRootOfNegativeNumberError();
        else if (*this <= one) return *this;
        BigInteger l(0), r = *this;
        BigInteger ans(0);
        while (l <= r) {
            BigInteger tmp = l + r;
            BigInteger mid = tmp / two;
            if (mid * mid == *this) return mid;
            if (mid * mid < *this) {
                l = mid + one;
                ans = mid;
            } else r = mid - one;
        }
        return ans;
    }

    BigInteger operator%(const BigInteger &other) {
        if (this->isNaN || other.isNaN) throw NaN_ERROR();
        BigInteger result = *this - ((*this / other) * other);
        if (result.isNegative) result = result + other;
        return result;
    }

    std::string toString() const {
        const long tmp = (this->isNegative ? 1 : 0);
        std::string result(this->digits.size() + tmp, ' ');
        if (this->isNegative) result[0] = '-';
        for (long i = tmp; i < this->digits.size() + tmp; i++)
            result[i] = '0' + this->digits[this->digits.size() - 1 - (i - tmp)];
        return result;
    }
};

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("%s\n", "Wrong number of arguments");
        return 1;
    }
    FILE *in, *out;
    if (!(in = fopen(argv[1], "r"))) {
        printf("%s\n", "Can't open input file");
        return 1;
    }
    if (!(out = fopen(argv[2], "r"))) {
        printf("%s\n", "Can't open output file");
        fclose(in);
        return 1;
    }
    char a[INT_MAX], op[10];
    fscanf(in, "%s%s", a, op);
    BigInteger A((std::string(a)));
    bool wasError = false;
    try {
        if (!strcmp(op, "#")) {
            char b[INT_MAX];
            fscanf(in, "%s", b);
            BigInteger B((std::string(b)));
            if (!strcmp(op, "+")) fprintf(out, "%s\n", (A + B).toString().c_str());
            else if (!strcmp(op, "-")) fprintf(out, "%s\n", (A - B).toString().c_str());
            else if (!strcmp(op, "*")) fprintf(out, "%s\n", (A * B).toString().c_str());
            else if (!strcmp(op, "/")) fprintf(out, "%s\n", (A / B).toString().c_str());
            else if (!strcmp(op, ">")) fprintf(out, "%d\n", A > B);
            else if (!strcmp(op, "<")) fprintf(out, "%d\n", A < B);
            else if (!strcmp(op, "<=")) fprintf(out, "%d\n", A <= B);
            else if (!strcmp(op, ">=")) fprintf(out, "%d\n", A >= B);
            else if (!strcmp(op, "!=")) fprintf(out, "%d\n", A != B);
            else if (!strcmp(op, "==")) fprintf(out, "%d\n", A == B);
        } else fprintf(out, "%s\n", A.sqrt().toString().c_str());
    } catch (NaN_ERROR &nanError) {
        wasError = true;
        printf("%s\n", "nanError");
    } catch (DivisionByZeroError &divByZeroError) {
        wasError = true;
        printf("%s\n", "divError");
    } catch (SquareRootOfNegativeNumberError &sqrtError) {
        wasError = true;
        printf("%s\n", "sqrtError");
    }
    fclose(in);
    fclose(out);
    if (wasError) return 1;
    return 0;
}