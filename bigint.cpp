//Chloe Chung
//Project 6 - BigInt
//May 8, 2025

// I certify that this program is my own original work. I did not copy any part of this program from
// any other source. I further certify that I typed each and every line of code in this program.

#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;


class BigInt
{
    private:
        vector<char> digits;
    public:
        //default BigInt constructor = 0
        BigInt(){
            digits.push_back('0');
        }

        BigInt(int n) // 0 to 2.1 billion
        {
            if (n == 0)
                digits.push_back('0');
            
            while (n > 0)
            {
                char digit = n%10 + '0'; //strips ones place (last digit off) & converts to char using ASCII (not int)
                digits.push_back(digit); // pushs last place digit into vector, prints backward
                n /= 10;
            }
        }

        BigInt(string str)
        {
            for(int i = str.length() - 1; i >=0; --i)
            digits.push_back(str[i]);
        }

        BigInt operator+ (BigInt a)
        {
            BigInt totalSum;
            totalSum.digits.clear(); 

            int carry = 0;
            auto maxSize = max(digits.size(), a.digits.size());

            for(auto i = 0; i < maxSize || carry; ++i)
            {
                int digitA = (i < digits.size()) ? (digits[i] - '0') : 0; // i if i < # of digits, else treat it as 0 (int)
                int digitB = (i < a.digits.size()) ? (a.digits[i] - '0') : 0;
                int placeSum = digitA + digitB + carry;

                totalSum.digits.push_back(placeSum % 10 + '0');
                carry = placeSum / 10;
            };

            return totalSum;
        }

        BigInt operator+ (int n)
        {
            return *this + BigInt(n);
        }
        
        BigInt operator- (BigInt a)
        {
            BigInt totalDifference;

            int borrow = 1;
            for(auto i = 0; i < digits.size(); ++i)
            {
                int digitA = digits[i] - '0' - borrow; // i if i < # of digits, else treat it as 0
                int digitB = (i < a.digits.size()) ? (a.digits[i] - '0') : 0;
                if (digitA < digitB)
                {
                    digitA += 10;
                    borrow = 1;
                }else{
                    borrow = 0;
                }
                int placeDifference = digitA - digitB;
                totalDifference.digits.push_back(placeDifference + '0');
            };
            
            //remove leading (trailing) zeros
            while(totalDifference.digits.size() > 1 && totalDifference.digits.back() == '0')
                totalDifference.digits.pop_back();

            return totalDifference;
        }

        BigInt operator* (BigInt a)
        {
            BigInt totalProduct;
            totalProduct.digits.assign(digits.size() + a.digits.size(), '0');

            for(auto i = 0; i <= digits.size(); ++i)
            {
                int carry = 0;
                int digitA = digits[i] - '0';

                for (auto j = 0; j <= a.digits.size() || carry; ++j)
                {
                    int digitB = (j < a.digits.size()) ? (a.digits[j] - '0'): 0;
                    int sum = (totalProduct.digits[i + j] - '0') + digitA * digitB + carry;
                    totalProduct.digits[i+j] = (sum % 10) + '0';
                    carry = sum / 10;

                }
            }

            while(totalProduct.digits.size() > 1 && totalProduct.digits.back() == '0')
                totalProduct.digits.pop_back();
            return totalProduct;
        }

        BigInt operator/ (BigInt divisor) //FIXME
        {
            BigInt quotient;
            BigInt remainder;

            if(divisor == BigInt(0))
                throw runtime_error("Division by zero");

            quotient.digits.resize(digits.size(), '0');
            for(int i = digits.size() - 1; i>=0; --i)
            {
                remainder.digits.insert(remainder.digits.begin(), digits[i]);

                //clear out leading zeros
                while(remainder.digits.size() > 1 && remainder.digits.back() == '0')
                    remainder.digits.pop_back();

                //count number of times divisor fits into remainder
                int count = 0;
                while(remainder >= divisor)
                {
                    remainder = remainder - divisor;
                    ++count;
                }

                quotient.digits[i] = count + '0';
            }

            while(quotient.digits.size() > 1 && quotient.digits.back() == '0')
                quotient.digits.pop_back();

            return quotient;
        }

        bool operator== (const BigInt& a)
        {
            if(digits.size() != a.digits.size())
                return false;
            for(auto i = 0; i < digits.size(); ++i)
            {
                if(digits[i] != a.digits[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator >= (const BigInt& a)
        {
            if(digits.size() != a.digits.size())
                return digits.size() > a.digits.size();

            for(int i = digits.size() - 1; i >= 0; --i)
            {
                if(digits[i] < a.digits[i])
                    return false;
            }
            return true;
        }
        
        BigInt operator++(int a) 
        {
            BigInt temp = *this;
            *this = *this + BigInt(1);
            return temp;
        }

        BigInt operator++( ) 
        {
            *this = *this + BigInt(1);
            return *this;
        }


        BigInt operator% (BigInt a)
        {
            BigInt quotient = *this / a;
            BigInt mod = *this - (quotient * a);
            return mod;  
        }

        void print()
        {
            if(digits.size() == 1 && digits[0] == '0')
                cout << "0";
            for(int i = digits.size() - 1; i >=0; --i)
            {
                cout << digits[i];
            }
        }
        
        BigInt _fibo(BigInt n, BigInt a, BigInt b) const
        {
            if(n == BigInt(0)) return a;
            else if(n == BigInt(1)) return b;

            return _fibo(n-BigInt(1), b, a+b);
        }

        BigInt fibo() const
        {
            return _fibo(*this, BigInt(0), BigInt(1));
        }

        BigInt _fact(BigInt num) const
        {
            if (num == BigInt(0) || num == BigInt(1))
                return BigInt(1);
            return num * _fact(num - BigInt(1));
        }

        BigInt fact() const
        {
            return _fact(*this);
        }

        friend BigInt operator+ (int a, BigInt b) 
        {
            return BigInt(a) + b;
        }

        // can print backwards, use reverse operator, use reverse function
        friend ostream & operator<<(ostream & out, const BigInt & n) //overloads << operator
        {
            if(n.digits.size() <= 12)
            {
                for (auto it = n.digits.rbegin(); it != n.digits.rend(); ++it)
                    out << (*it - '0'); // have to typecast to an integer
            }
            else
            {
                //scientific notation up to 7 sig figs
                out << (*n.digits.rbegin() - '0') << '.'; //output first sig fig and decimal
                int count = 1;
                for (auto it = n.digits.rbegin() + 1; it != n.digits.rend() && count < 7; ++it)
                    {
                    out << (*it - '0'); // have to typecast to an integer
                    count++; //increment count of sig figs printed
                    }
                int exponent = n.digits.size() - 1; //exponent = total size - 1
                out << 'e' << exponent;
            }

            return out;
        }

        // BigInt operator– (int);
        // BigInt operator[](int); // index function
        // int size();
};

void testUnit()
{
    int space = 10;
    cout << "\a\nTestUnit:\n"<<flush;
    system("whoami");
    system("date");
    // initialize variables
    BigInt n1(25);
    BigInt s1("25");
    BigInt n2(1234);
    BigInt s2("1234");
    BigInt n3(n2);
    BigInt fibo(12345);
    BigInt fact(50);
    BigInt imax = INT_MAX;
    BigInt big("9223372036854775807");
    // display variables
    cout << "n1(int)    :"<<setw(space)<<n1<<endl;
    cout << "s1(str)    :"<<setw(space)<<s1<<endl;
    cout << "n2(int)    :"<<setw(space)<<n2<<endl;
    cout << "s2(str)    :"<<setw(space)<<s2<<endl;
    cout << "n3(n2)     :"<<setw(space)<<n3<<endl;
    cout << "fibo(12345):"<<setw(space)<<fibo<<endl;
    cout << "fact(50)   :"<<setw(space)<<fact<<endl;
    cout << "imax       :"<<setw(space)<<imax<<endl;
    cout << "big        :"<<setw(space)<<big<<endl;
    cout << "big.print(): "; big.print(); cout << endl;

    cout << n2 << "/"<< n1<< " = " << n2/n1 << " rem "<<n2%n1<<endl;
    cout << "fibo("<<fibo<<") = "<<fibo.fibo() << endl;
    cout << "fact("<<fact<<") = "<<fact.fact() << endl;
    cout << "10 + n1 = " << 10+n1 << endl;
    cout << "n1 + 10 = " << n1+10 << endl;
    cout << "(n1 == s1)? --> "<<((n1==s1)?"true":"false")<<endl;
    cout << "n1++ = ? --> before:"<<n1++<<" after:"<<n1<<endl;
    cout << "++s1 = ? --> before:"<<++s1<<" after:"<<s1<<endl;
    cout << "s2 * big = ? --> "<< s2 * big<<endl;
    cout << "big * s2 = ? --> "<< big * s2<<endl;
};

int main()
{
    testUnit();

    return 0;

};