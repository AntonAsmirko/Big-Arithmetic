#include <stdio.h>
#include <string>

#define NULL_STR (string) "NULL"

class Big_Integer
{
private:
    size_t size;
    bool is_lower_zero;
    unsigned char *digits;

    Big_Integer(size_t size)
    {
        this->digits = new unsigned char[size];
        this->size = size;
    }

    void fill(std::string num, int untlill)
    {
        for (int i = num.size() - 1; i >= 0; i++)
        {
            this->digits[this->size - i] = num[i];
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

    bool equals(Big_Integer *another)
    {
        return this->compare_to(another) == 0;
    }

    Big_Integer *add(Big_Integer *another)
    {
        Big_Integer *max;

        if (this->compare_to(another) == 1)
        {
            max = this;
        }
        else
        {
            max = another;
        }

        size_t new_size = max->size;

        if (this->is_negative() && !another->is_negative())
        {
        }
        else if (!this->is_negative() && another->is_negative())
        {
        }
        else if (!this->is_negative() && !another->is_negative())
        {
        }
        else
        {
        }
    }

    ~Big_Integer()
    {
        delete[] digits;
    }
};

int main()
{
    Big_Integer *my_big = new Big_Integer("12345");
    return 0;
}