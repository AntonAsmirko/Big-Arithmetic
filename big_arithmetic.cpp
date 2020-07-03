#include <stdio.h>

size_t str_len(const char *str)
{
    size_t len = 0;
    for (; str[len] != '\0'; len++)
        ;
    return len;
}

class Big_Integer
{
private:
    size_t size;
    bool is_lower_zero;
    unsigned char *digits;

    void allocate_and_fill(char *num, int untill)
    {
        this->size = str_len(num);
        this->digits = new unsigned char[size];
        for (int i = size; i >= untill; i--)
        {
            this->digits[size - i - 1] = num[i] - '0';
        }
    }

public:
    Big_Integer(char *num)
    {
        if (num[0] != '-')
        {
            this->is_lower_zero = false;
            allocate_and_fill(num, 0);
        }
        else
        {
            this->is_lower_zero = true;
            allocate_and_fill(num, 1);
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

    int compare_to(const Big_Integer &another)
    {
        int this_size = size;
        int another_size = another.size;

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
            if (another.is_lower_zero)
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
            if (this->digits[this_size] > another.digits[this_size])
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
            else if (this->digits[this_size] < another.digits[this_size])
            {
                if (another.is_lower_zero)
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

    ~Big_Integer()
    {
        delete[] digits;
    }
};

int main()
{
    Big_Integer *my_big = new Big_Integer("12345");
    printf("%d", my_big->is_negative());
    return 0;
}