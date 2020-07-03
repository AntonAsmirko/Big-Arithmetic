#include <string>
#include <cstring>
#include <iostream>
#include <utility>

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

    int bool_to_int(bool statement)
    {
        if (statement)
        {
            return 1;
        }
        return -1;
    }

    /**
     *  @param Big_Integer another num
     *  @returns 
     * -1 if magnitude of another number greater than magnitude of this,
     *  0 if magnitudes are equals,
     *  1 if magnitude of this greater than magnitude of another
     **/

    int compare_magn(Big_Integer *another)
    {
        if (size > another->size)
        {
            return bool_to_int(!this->is_lower_zero);
        }
        else if (size < another->size)
        {
            bool_to_int(another->is_lower_zero);
        }

        for (int i = size; i >= 0; i--)
        {
            if (this->digits[i] > another->digits[i])
            {
                bool_to_int(!this->is_lower_zero);
            }
            else if (this->digits[i] < another->digits[i])
            {
                bool_to_int(another->is_lower_zero);
            }
        }
        return 0;
    }

    /**
     * @param second - BigInt which will be compared with this object
     * @return pair {bigger, smaller}
     **/

    std::pair<Big_Integer *, Big_Integer *> max_and_min(Big_Integer *second)
    {
        Big_Integer *bigger = this;
        Big_Integer *smaller = second;

        if (this->compare_magn(second) == -1)
        {
            bigger = second;
            smaller = this;
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

    static void add_logic(Big_Integer *bigger, Big_Integer *smaller, Big_Integer *result, Big_Integer *origin)
    {
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

        //result->is_lower_zero = !(bigger == origin);
    }

    static void subtruct_logic(Big_Integer *bigger, Big_Integer *smaller, Big_Integer *result, Big_Integer *origin)
    {
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
        for (int i = smaller->size; owe != 0; i++)
        {
            result->digits[i] -= owe;
            if (result->digits[i] < 0)
            {
                owe = 1;
            }
        }
        result->is_lower_zero = !(bigger == origin);
    }

    Big_Integer *add_subtruct_utill(Big_Integer *another, void (*logic)(Big_Integer *bigger, Big_Integer *smaller, Big_Integer *result, Big_Integer *origin))
    {
        std::pair<Big_Integer *, Big_Integer *> bigger_and_smaller = max_and_min(another);
        Big_Integer *result = Big_Integer::create_and_init(MAX(this->size, another->size) + 1, bigger_and_smaller.first);
        logic(bigger_and_smaller.first, bigger_and_smaller.second, result, this);
        return result;
    }

    bool equals(Big_Integer *another)
    {
        return this->compare_magn(another) == 0;
    }

    Big_Integer *subtruct(Big_Integer *another)
    {
        if (this->is_lower_zero && !another->is_lower_zero)
        {
            Big_Integer *result = add_subtruct_utill(another, Big_Integer::subtruct_logic);
            result->is_lower_zero = true;
            return result;
        }
        else if (!this->is_lower_zero && !another->is_lower_zero)
        {
            return add_subtruct_utill(another, Big_Integer::subtruct_logic);
        }
        else if (!this->is_lower_zero && another->is_lower_zero)
        {
            return add_subtruct_utill(another, Big_Integer::add_logic);
        }
        else
        {
            Big_Integer *result = add_subtruct_utill(another, Big_Integer::subtruct_logic);

            if (this->is_lower_zero && this->compare_magn(another) == 1)
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

        Big_Integer *res = A->subtruct(B);

        std::cout << res->to_string() << std::endl;
    }
    return 0;
}