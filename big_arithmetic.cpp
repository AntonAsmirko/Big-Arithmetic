#include <string>
#include <cstring>
#include <iostream>

#define MAX(a, b) (a > b ? a : b)
#define DATA_TYPE unsigned char

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

    /**
     *  @param Big_Integer another num
     *  @returns -1 if another number greater than this,
     *  0 if they are equals,
     *  1 if this greater than another
     **/

    int compare_to(Big_Integer *another)
    {
        int this_size = size;
        int another_size = another->size;

        if (this_size > another_size)
        {
            if (this->is_negative())
            {
                return -1;
            }
            else
            {
                return 1;
            }
        }
        else if (this_size < another_size)
        {
            if (another->is_negative())
            {
                return 1;
            }
            else
            {
                return -1;
            }
        }

        for (; this_size >= 0; this_size--)
        {
            if (this->digits[this_size] > another->digits[this_size])
            {
                if (this->is_negative())
                {
                    return -1;
                }
                else
                {
                    return 1;
                }
            }
            else if (this->digits[this_size] < another->digits[this_size])
            {
                if (another->is_negative())
                {
                    return 1;
                }
                else
                {
                    return -1;
                }
            }
        }

        return 0;
    }

    Big_Integer *add_utill(Big_Integer *another)
    {
        size_t res_len = MAX(this->size, another->size) + 1;

        Big_Integer *bigger = this;
        Big_Integer *smaller = another;

        if (this->compare_to(another) == -1)
        {
            bigger = another;
            smaller = this;
        }

        Big_Integer *result = new Big_Integer(res_len);
        std::memcpy(result->digits, bigger->digits, sizeof(DATA_TYPE) * bigger->size);
        result->is_lower_zero = bigger->is_lower_zero;

        int carry = 0;
        for (int i = 0; i < smaller->size; i++)
        {
            int res = result->digits[i] + smaller->digits[i] + carry;
            carry = res / 10;
            res %= 10;
            result->digits[i] = res;
        }

        int i = smaller->size;

        do
        {
            result->digits[i] += carry;
            i++;
        } while (result->digits[i - 1] >= 10);

        return result;
    }

    Big_Integer *subtruct_utill(Big_Integer *another)
    {
        size_t res_len = MAX(this->size, another->size);

        Big_Integer *bigger = this;
        Big_Integer *smaller = another;

        if (this->compare_to(another) == -1)
        {
            bigger = another;
            smaller = this;
        }

        Big_Integer *result = new Big_Integer(res_len);
        std::memcpy(result->digits, bigger->digits, sizeof(DATA_TYPE) * bigger->size);

        int owe = 0;
        for (int i = 0; i < smaller->size; i++)
        {
            if (result->digits[i] >= smaller->digits[i] + owe)
            {
                result->digits[i] -= (smaller->digits[i] + owe);
            }
            else
            {
                owe = 1;
                result->digits[i] = result->digits[i] - smaller->digits[i] + 10;
            }
        }

        int i = smaller->size;

        do
        {
            result->digits[i] -= owe;
            if (result->digits[i] < 0)
            {
                owe = 1;
            }
            i++;
        } while (owe != 0);

        if (bigger == this)
        {
            result->is_lower_zero = false;
        }
        else
        {
            result->is_lower_zero = true;
        }

        return result;
    }

    bool equals(Big_Integer *another)
    {
        return this->compare_to(another) == 0;
    }

    Big_Integer *subtruct(Big_Integer *another)
    {
        if (this->is_negative() && !another->is_negative())
        {
            Big_Integer *result = subtruct_utill(another);
            result->is_lower_zero = true;
            return result;
        }
        else if (!this->is_negative() && !another->is_negative())
        {
            return subtruct_utill(another);
        }
        else if (!this->is_negative() && another->is_negative())
        {
            return add_utill(another);
        }
        else
        {
            Big_Integer *result = subtruct_utill(another);

            if (this->is_negative() && this->compare_to(another) == 1)
            {
                result->is_lower_zero = true;
            }
            else
            {
                result->is_lower_zero = false;
            }
            return result;
        }
    }

    std::string to_string()
    {
        const int tmp = (this->is_negative() ? 1 : 0);
        std::string result(this->size + tmp, ' ');
        size_t i = 0;
        if (this->is_negative())
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

        Big_Integer *res = A->subtruct(B);

        std::cout << res->to_string() << std::endl;
    }
    return 0;
}