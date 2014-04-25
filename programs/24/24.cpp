#include <iostream>
#include <cstdio>
#include <vector>

using namespace std;

#define EXTRA_DBG

void print_vector(const vector<int> &in)
{
    cout << "[";
    for(int i=0; i<in.size()-1 ; i++) {
        cout << in[i] << ", ";
    }
    cout << in[in.size()-1] << "]" << endl;
}

void swap(vector<int> &in, int pos1, int pos2)
{
    if(pos1 >= in.size() || pos2 >= in.size())
        return;
    if(pos1 == pos2)
        return;

    int temp = in[pos1];
    in[pos1] = in[pos2];
    in[pos2] = temp;
}

typedef void (calc_cb)(vector<int> &num, vector<int> &factor);

#define ADD 0
#define SUB 1
#define MUL 2
#define DIV 3
/* factor chars */
const char fac_ch [] = {'+', '-', '*', '/'};

int calc(int num1, int num2, int factor)
{
    switch(factor){
    case ADD:
        return (num1 + num2);
    case SUB:
        return (num1 - num2);
    case MUL:
        return (num1 * num2);
    case DIV:
        if(num2 == 0 || (num1 % num2) != 0)
            return 100000;
        else if(num1 % num2 ==0)
            return (num1/num2);
    default:
        return 1000000;
    }
}

/* 
 * Once we get a sequence [a,b,c,d]
 * There are 5 ways we can use them to compute:
 *  1)  ((a ? b) ? c) ? d 
 *             ?
 *            / \
 *           ?   d
 *          / \
 *         ?   c
 *        / \
 *       a   b
 */
void type1(vector<int> &num, vector<int> &fac)
{
    int res;

    if(num.size() != 4 || fac.size() != 3)
        return;
    
    res = calc(num[0], num[1], fac[0]);
    res = calc(res, num[2], fac[1]);
    res = calc(res, num[3], fac[2]);

    if(res ==  24) {
        printf("((%d %c %d) %c %d) %c %d = 24\n",
                num[0], fac_ch[fac[0]], num[1], fac_ch[fac[1]], 
                num[2], fac_ch[fac[2]], num[3]);
    }
}
/*
 *  2)  (a ? b) ? (c ? d)
 *             ?
 *           /   \
 *         ?       ?   
 *        / \     / \
 *       a   b   c   d 
 */
void type2(vector<int> &num, vector<int> &fac)
{
    int res1, res2, res;

    if(num.size() != 4 || fac.size() != 3)
        return;
    
    res1 = calc(num[0], num[1], fac[0]);
    res2 = calc(num[2], num[3], fac[2]);
    res = calc(res1, res2, fac[1]);

    if(res ==  24) {
        printf("(%d %c %d) %c (%d %c %d) = 24\n",
                num[0], fac_ch[fac[0]], num[1], fac_ch[fac[1]], 
                num[2], fac_ch[fac[2]], num[3]);
    }
}
/*
 *  3)  (a ? (b ? c)) ? d 
 *            ?
 *           / \
 *          ?   d
 *         / \
 *        a   ? 
 *           / \
 *          b   c 
 */
void type3(vector<int> &num, vector<int> &fac)
{
    int res;

    if(num.size() != 4 || fac.size() != 3)
        return;
    
    res = calc(num[1], num[2], fac[1]);
    res = calc(num[0], res, fac[0]);
    res = calc(res, num[3], fac[2]);

    if(res ==  24) {
        printf("(%d %c (%d %c %d)) %c %d = 24\n",
                num[0], fac_ch[fac[0]], num[1], fac_ch[fac[1]], 
                num[2], fac_ch[fac[2]], num[3]);
    }
}
/*
 *  4)  a ? ((b ? c) ? d)
 *            ?
 *           / \
 *          a   ? 
 *             / \
 *            ?   d
 *           / \
 *          b   c 
 */
void type4(vector<int> &num, vector<int> &fac)
{
    int res;

    if(num.size() != 4 || fac.size() != 3)
        return;
    
    res = calc(num[1], num[2], fac[1]);
    res = calc(res, num[3], fac[2]);
    res = calc(num[0], res, fac[0]);

    if(res ==  24) {
        printf("%d %c ((%d %c %d) %c %d) = 24\n",
                num[0], fac_ch[fac[0]], num[1], fac_ch[fac[1]], 
                num[2], fac_ch[fac[2]], num[3]);
    }
}
/*
 *  5)  a ? (b ? (c ? d))
 *            ?
 *           / \
 *          a   ? 
 *             / \
 *            b   ? 
 *               / \
 *              c   d 
 */
void type5(vector<int> &num, vector<int> &fac)
{
    int res;

    if(num.size() != 4 || fac.size() != 3)
        return;
    
    res = calc(num[2], num[3], fac[2]);
    res = calc(num[1], res, fac[1]);
    res = calc(num[0], res, fac[0]);

    if(res ==  24) {
        printf("%d %c (%d %c (%d %c %d)) = 24\n",
                num[0], fac_ch[fac[0]], num[1], fac_ch[fac[1]], 
                num[2], fac_ch[fac[2]], num[3]);
    }
}

vector<calc_cb*> calc_cbs;

void init()
{
    calc_cbs.push_back(type1);
    calc_cbs.push_back(type2);
    calc_cbs.push_back(type3);
    calc_cbs.push_back(type4);
    calc_cbs.push_back(type5);
}


void calc_engine(vector<int> &in)
{
    vector<int> factors;

    for(int i=0; i<64; i++) {
        factors.clear();
        // three factors
        factors.push_back(i >> 4);
        factors.push_back((i >> 2) & 0x03);
        factors.push_back(i & 0x03);
        for(int j=0; j<calc_cbs.size() ; j++)
            calc_cbs[j](in, factors);
    }
}

int count = 1;
// fixed_size is the size at the head of the vector that are fixed
void get_seq(vector<int> &in, int fixed_size)
{
    vector<int> array;

    if(fixed_size == 3) {
        //cout << count++ << ": ";
        //print_vector(in);
        // now we get a sequence, send it calc engine
        calc_engine(in);
        return;
    }

    // we should change in[fixed_size] for (4-fixed_size) times
    for(int i=0; i<(4-fixed_size) ; i++) {
        array = in;
        swap(array, fixed_size, fixed_size + i);
    
        get_seq(array, fixed_size + 1);
    }
}

#if 0
void calc_24(vector<int> &in)
{
    vector<int> fixed, other;

    for(int i=0; i<4 ; i++) {
        fixed.clear();
        fixed
        for(int j=0; j<4 ; j++)
    
        get_seq(fixed, other);
    }
}
#endif

int main(int argc, char **argv)
{
    int i, num;
    vector<int>     nums;

    init();

    // get input
    for(i=0; i<4 ; i++) {
        cout << "Input number " << (i+1) << ": ";
        cin >> num;
        nums.push_back(num);
    }

#ifdef EXTRA_DBG
    //print_vector(nums);
#endif


    get_seq(nums, 0);


    return 0;
}

#if 0
class Rational
{
public:
    Rational() : numerator(0), denominator(1) {}
    Rational(int n, int d) : numerator(n), denominator(d) {}
    Rational(int n) : numerator(n), denominator(1) {}
    Rational(const Rational &r) : numerator(r.numerator), denominator(r.denominator) {}
    Rational operator=(const Rational &r) {
        if(this == &r) return *this;
        numerator = r.numerator;
        denominator = r.denominator;
    }

friend Rational operator+(const Rational &l, const Rational &r);
friend Rational operator-(const Rational &l, const Rational &r);
friend Rational operator*(const Rational &l, const Rational &r);
friend Rational operator/(const Rational &l, const Rational &r);

private:
    int numerator;
    int denominator;
}

Rational operator+(const Rational &l, const Rational &r)
{
    Rational res(l);
    
}

void test_rational()
{
    Rational a(1,4), b(3,4);
}
#endif
