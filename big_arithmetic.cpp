#include <string>
#include <cstring>
#include <iostream>
#include <utility>
#include <cmath>
#include <functional>

#define MAX(a, b) (a > b ? a : b)
#define DATA_TYPE char

class Big_Integer
{
private:
    size_t size;
    bool is_lower_zero;
    DATA_TYPE *digits;

    Big_Integer(size_t size)
    {
        this->digits = (DATA_TYPE *)malloc(sizeof(DATA_TYPE) * size);
        this->size = size;
    }

    void fill(std::string num, int untlill)
    {
        for (int i = num.length() - 1; i >= untlill; i--)
        {
            this->digits[this->size - i - 1] = num[i] - '0';
        }
    }

    int compare_magn(Big_Integer *another)
    {
        if (size > another->size)
        {
            return 1;
        }
        else if (size < another->size)
        {
            return -1;
        }

        for (int i = size; i >= 0; i--)
        {
            if (this->digits[i] > another->digits[i])
            {
                return 1;
            }
            else if (this->digits[i] < another->digits[i])
            {
                return -1;
            }
        }
        return 0;
    }

    static std::pair<Big_Integer *, Big_Integer *> max_and_min(Big_Integer *first, Big_Integer *second)
    {
        Big_Integer *bigger = first;
        Big_Integer *smaller = second;

        if (first->compare_magn(second) == -1)
        {
            bigger = second;
            smaller = first;
        }
        return {bigger, smaller};
    }

    static Big_Integer *create_and_init(size_t len, Big_Integer *from)
    {
        Big_Integer *result = new Big_Integer(len);
        std::memcpy(result->digits, from->digits, sizeof(DATA_TYPE) * from->size);

        return result;
    }

public:
    Big_Integer(std::string num) : Big_Integer(num.length())
    {

        if (num[0] != '-')
        {
            this->is_lower_zero = false;
            fill(num, 0);
        }
        else
        {
            this->is_lower_zero = true;
            fill(num, 1);
        }
    }

    bool is_negative()
    {
        return this->is_lower_zero;
    }

    static void abs_sum(Big_Integer *bigger, Big_Integer *smaller, Big_Integer *result)
    {
        int carry = 0;
        for (int i = 0; i < smaller->size; i++)
        {
            int res = result->digits[i] + smaller->digits[i] + carry;
            carry = res / 10;
            res %= 10;
            result->digits[i] = res;
        }
        for (int i = smaller->size; carry != 0; i++)
        {
            result->digits[i] += carry;
            carry = result->digits[i] / 10;
            result->digits[i] %= 10;
        }
    }

    static void abs_diff(Big_Integer *bigger, Big_Integer *smaller, Big_Integer *result)
    {
        int owe = 0;
        for (int i = 0; i < smaller->size; i++)
        {
            result->digits[i] -= (smaller->digits[i] + owe);
            if (result->digits[i] >= 0)
            {
                owe = 0;
            }
            else
            {
                result->digits[i] += 10;
                owe = 1;
            }
        }
        for (int i = smaller->size; owe != 0; i++)
        {
            result->digits[i] -= owe;
            if (result->digits[i] < 0)
            {
                result->digits[i] += 10;
                owe = 1;
            }
            else
            {
                owe = 0;
            }
        }
    }

    static Big_Integer *add_subtruct_utill(Big_Integer *first, Big_Integer *second,
                                           void (*logic)(Big_Integer *bigger, Big_Integer *smaller, Big_Integer *result))
    {
        std::pair<Big_Integer *, Big_Integer *> bigger_and_smaller = Big_Integer::max_and_min(first, second);
        Big_Integer *result = Big_Integer::create_and_init(MAX(first->size, second->size) + 1, bigger_and_smaller.first);
        logic(bigger_and_smaller.first, bigger_and_smaller.second, result);
        return result;
    }

    bool equals(Big_Integer *another)
    {
        return this->compare_magn(another) == 0;
    }

    Big_Integer *common_utill(Big_Integer *first, Big_Integer *second, bool predicate,
                              void (*logic)(Big_Integer *bigger, Big_Integer *smaller, Big_Integer *result))
    {
        auto tmp = add_subtruct_utill(first, second, logic);
        tmp->is_lower_zero = predicate;
        return tmp;
    }

    Big_Integer *subtruct(Big_Integer *another)
    {
        if (this->is_lower_zero && another->is_lower_zero)
            return common_utill(this, another, compare_magn(another) == 1, Big_Integer::abs_diff);
        else if (!this->is_lower_zero && !another->is_lower_zero)
            return common_utill(this, another, compare_magn(another) == -1, Big_Integer::abs_diff);
        else if (this->is_lower_zero && !another->is_lower_zero)
            return common_utill(this, another, true, Big_Integer::abs_sum);
        else if (!this->is_lower_zero && another->is_lower_zero)
            return common_utill(this, another, false, Big_Integer::abs_sum);
    }

    Big_Integer *flip_sign_and_make_op(Big_Integer *another, std::function<Big_Integer *(Big_Integer *arg)> op)
    {
        another->is_lower_zero = !another->is_lower_zero;
        Big_Integer *result = op(another);
        another->is_lower_zero = !another->is_lower_zero;
        return result;
    }

    Big_Integer *add(Big_Integer *another)
    {
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

    std::string to_string()
    {

        const int tmp = (this->is_lower_zero ? 1 : 0);
        std::string result(this->size + tmp, ' ');
        size_t i = 0;
        if (this->is_lower_zero)
        {
            result[i] = '-';
            i++;
        }
        for (; i < this->size + tmp; i++)
        {
            result[i] = '0' + this->digits[this->size - 1 - (i - tmp)];
        }

        return result;
    }

    ~Big_Integer()
    {
        delete[] digits;
    }
};

int main()
{
    for (int i = 0; i < 100; i++)
    {
        std::string a, b;
        std::cin >> a >> b;
        Big_Integer *A = new Big_Integer(a);
        Big_Integer *B = new Big_Integer(b);

        Big_Integer *res = A->add(B);

        std::cout << res->to_string() << std::endl;
    }
    return 0;
}