#include <string>
#include <cstring>
#include <iostream>
#include <utility>
#include <cmath>
#include <functional>
#include <vector>

#define MAX(a, b) (a > b ? a : b)
#define DATA_TYPE char
#define DATA_TYPE_ZERO 0
#define INTERNAL_BASE 10

class Big_Integer {
private:
    size_t size;
    bool is_lower_zero;
    DATA_TYPE *digits;

    Big_Integer(size_t size) {
        this->digits = (DATA_TYPE *) malloc(sizeof(DATA_TYPE) * size);
        this->size = size;
    }

    void fill(std::string num, int untlill) {
        for (int i = num.length() - 1; i >= untlill; i--) {
            this->digits[this->size - i - (1 - untlill)] = num[i] - '0';
        }
    }

    int compare_magn(Big_Integer *another) {
        if (size > another->size) {
            return 1;
        } else if (size < another->size) {
            return -1;
        }

        for (int i = size; i >= 0; i--) {
            if (this->digits[i] > another->digits[i]) {
                return 1;
            } else if (this->digits[i] < another->digits[i]) {
                return -1;
            }
        }
        return 0;
    }

    static std::pair<Big_Integer *, Big_Integer *> max_and_min(Big_Integer *first, Big_Integer *second) {
        Big_Integer *bigger = first;
        Big_Integer *smaller = second;

        if (first->compare_magn(second) == -1) {
            bigger = second;
            smaller = first;
        }
        return {bigger, smaller};
    }

    static Big_Integer *create_and_init(size_t len, Big_Integer *from) {
        Big_Integer *result = new Big_Integer(len);
        std::memcpy(result->digits, from->digits, sizeof(DATA_TYPE) * from->size);
        result->size = len;
        return result;
    }

    static int shrink(Big_Integer *num) {
        int cur = num->size - 1;
        for (; num->digits[cur] == 0 && cur >= 0; cur--);
        if (cur <= 0) cur = 1;
        else cur++;
        if (cur >= 0 && ((num->digits = (DATA_TYPE *) realloc(num->digits, cur)) == NULL)) {
            return 1;
        }
        num->size = cur;
        return 0;
    }

    static void abs_sum(Big_Integer *bigger, Big_Integer *smaller, Big_Integer *result) {
        int carry = 0;
        for (int i = 0; i < smaller->size; i++) {
            int res = result->digits[i] + smaller->digits[i] + carry;
            carry = res / 10;
            res %= 10;
            result->digits[i] = res;
        }
        for (int i = smaller->size; carry != 0; i++) {
            result->digits[i] += carry;
            carry = result->digits[i] / 10;
            result->digits[i] %= 10;
        }
    }

    static void abs_diff(Big_Integer *bigger, Big_Integer *smaller, Big_Integer *result) {
        int owe = 0;
        int i = 0;
        for (; i < smaller->size; i++) {
            result->digits[i] -= (smaller->digits[i] + owe);
            if (result->digits[i] >= 0) {
                owe = 0;
            } else {
                result->digits[i] += 10;
                owe = 1;
            }
        }
        if (i != bigger->size) {
            for (i = smaller->size; owe != 0 && i < bigger->size; i++) {
                result->digits[i] -= owe;
                if (result->digits[i] < 0) {
                    result->digits[i] += 10;
                    owe = 1;
                } else {
                    owe = 0;
                }
            }
        }
        result->digits[i] = abs(result->digits[i]);
    }

    static Big_Integer *add_subtruct_utill(Big_Integer *first, Big_Integer *second,
                                           void (*logic)(Big_Integer *bigger, Big_Integer *smaller,
                                                         Big_Integer *result)) {
        std::pair<Big_Integer *, Big_Integer *> bigger_and_smaller = Big_Integer::max_and_min(first, second);
        Big_Integer *result = Big_Integer::create_and_init(MAX(first->size, second->size) + 1,
                                                           bigger_and_smaller.first);
        logic(bigger_and_smaller.first, bigger_and_smaller.second, result);
        Big_Integer::shrink(result);
        return result;
    }

    Big_Integer *common_utill(Big_Integer *first, Big_Integer *second, bool predicate,
                              void (*logic)(Big_Integer *bigger, Big_Integer *smaller, Big_Integer *result)) {
        auto tmp = add_subtruct_utill(first, second, logic);
        tmp->is_lower_zero = predicate;
        return tmp;
    }

    Big_Integer *flip_sign_and_make_op(Big_Integer *another, std::function<Big_Integer *(Big_Integer *arg)> op) {
        another->is_lower_zero = !another->is_lower_zero;
        Big_Integer *result = op(another);
        another->is_lower_zero = !another->is_lower_zero;
        return result;
    }

    int compare(Big_Integer *another) {
        if (this->is_lower_zero && !another->is_lower_zero)
            return -1;
        if (!this->is_lower_zero && another->is_lower_zero)
            return 1;
        int compare_res = compare_magn(another);
        if (compare_res == 1) {
            if (this->is_lower_zero && another->is_lower_zero)
                return -1;
            else if (!this->is_lower_zero && !another->is_lower_zero)
                return 1;
        } else if (compare_res == -1) {
            if (this->is_lower_zero && another->is_lower_zero)
                return 1;
            else if (!this->is_lower_zero && !another->is_lower_zero)
                return -1;
        }
        return 0;
    }

public:
    Big_Integer(std::string num) : Big_Integer(num.length() - (num[0] != '-' ? 0 : 1)) {
        this->is_lower_zero = num[0] == '-';
        fill(num, num[0] != '-' ? 0 : 1);
    }

    bool operator==(Big_Integer *other) { return compare(other) == 0; }

    bool operator<(Big_Integer *other) { return compare(other) == -1; }

    bool operator>(Big_Integer *other) { return compare(other) == 1; }

    bool operator>=(Big_Integer *other) { return this > other || this == other; }

    bool operator<=(Big_Integer *other) { return this < other || this == other; }

    bool operator!=(Big_Integer *other) { return this < other || this > other; }

    Big_Integer *subtruct(Big_Integer *another) {
        if (this->is_lower_zero && another->is_lower_zero)
            return common_utill(this, another, compare_magn(another) == 1, Big_Integer::abs_diff);
        else if (!this->is_lower_zero && !another->is_lower_zero)
            return common_utill(this, another, compare_magn(another) == -1, Big_Integer::abs_diff);
        else if (this->is_lower_zero && !another->is_lower_zero)
            return common_utill(this, another, true, Big_Integer::abs_sum);
        else if (!this->is_lower_zero && another->is_lower_zero)
            return common_utill(this, another, false, Big_Integer::abs_sum);
    }

    Big_Integer *add(Big_Integer *another) {
        if (!this->is_lower_zero && !another->is_lower_zero)
            return common_utill(this, another, false, Big_Integer::abs_sum);
        else if (this->is_lower_zero && another->is_lower_zero)
            return common_utill(this, another, true, Big_Integer::abs_sum);
        else if (!this->is_lower_zero && another->is_lower_zero)
            return flip_sign_and_make_op(another, [another, this](Big_Integer *arg) {
                return common_utill(this, another, compare_magn(another) == -1, Big_Integer::abs_diff);
            });
        else
            return flip_sign_and_make_op(another, [another, this](Big_Integer *arg) {
                return common_utill(this, another, compare_magn(another) == 1, Big_Integer::abs_diff);
            });
    }

    Big_Integer *multiply(Big_Integer *another) {
        const size_t res_size = sizeof(DATA_TYPE) * (this->size + another->size);
        Big_Integer *result = new Big_Integer(res_size);
        std::memset(result->digits, DATA_TYPE_ZERO, sizeof(this->size + another->size));
        for (int i = 0; i < another->size; i++) {
            DATA_TYPE carry = 0;
            for (int j = 0; j < this->size; j++) {
                result->digits[i + j] += carry + this->digits[j] * another->digits[i];
                carry = result->digits[i + j] / INTERNAL_BASE;
                result->digits[i + j] %= INTERNAL_BASE;
            }
            result->digits[i + this->size] = carry;
        }
        result->is_lower_zero = this->is_lower_zero ^ another->is_lower_zero;
        Big_Integer::shrink(result);
        return result;
    }

    Big_Integer *shift_right() {
        Big_Integer *result = Big_Integer::create_and_init(this->size + 1, this);
        result->digits[this->size] = result->digits[this->size - 1];
        for (int i = result->size - 2; i > 0; --i) result->digits[i] = result->digits[i - 1];
        result->digits[0] = 0;
        return result;
    }

    Big_Integer *divide(Big_Integer *right) {
        Big_Integer *b = right;
        b->is_lower_zero = false;
        Big_Integer *result = new Big_Integer(this->size), *current = new Big_Integer("0");
        for (long long i = static_cast<long long>(this->size) - 1; i >= 0; --i) {
            current = current->shift_right();
            current->digits[0] = this->digits[i];
            Big_Integer::shrink(current);
            int x = 0, l = 0, r = INTERNAL_BASE;
            while (l <= r) {
                int m = (l + r) / 2;
                Big_Integer *t = b->multiply(new Big_Integer(std::to_string(m)));
                if (t <= current) {
                    x = m;
                    l = m + 1;
                } else r = m - 1;
            }

            result->digits[i] = x;
            auto tmp = b->multiply(new Big_Integer(std::to_string(x)));
            current = current->subtruct(tmp);
        }

        result->is_lower_zero = this->is_lower_zero != right->is_lower_zero;
        Big_Integer::shrink(result);
        return result;
    }

    std::string to_string() {

        const int tmp = (this->is_lower_zero ? 1 : 0);
        std::string result(this->size + tmp, ' ');
        size_t i = 0;
        if (this->is_lower_zero) {
            result[i] = '-';
            i++;
        }
        for (; i < this->size + tmp; i++) {
            result[i] = '0' + this->digits[this->size - 1 - (i - tmp)];
        }

        return result;
    }

    ~Big_Integer() {
        delete[] digits;
    }
};

int main() {
    std::string a, b;
    std::cin >> a >> b;
    auto *A = new Big_Integer(a);
    auto *B = new Big_Integer(b);
    std::cout << A->to_string()<< "<"<<B->to_string()<< " "<< (A < B)<< std::endl;
    std::cout << A->to_string()<< ">"<<B->to_string()<< " "<< (A > B)<< std::endl;
    std::cout << A->to_string()<< "=="<<B->to_string()<< " "<< (A == B)<< std::endl;
    std::cout << A->to_string()<< "<="<<B->to_string()<< " "<< (A <= B)<< std::endl;
    std::cout << A->to_string()<< ">="<<B->to_string()<< " "<< (A >= B)<< std::endl;
    std::cout << A->to_string()<< "!="<<B->to_string()<< " "<< (A != B)<< std::endl;
    return 0;
}