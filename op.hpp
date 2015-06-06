//Function pointer type definitions.
//* ISSUE: Just for ints right now
typedef int(*Uptr)(int);
typedef int(*Bptr)(int, int);

struct Uop {
    Uptr fn;
    Uop(Uptr u):fn(u){}
};

struct Bop {
    Bptr fn;
    int lvl, ass;
    Bop(Bptr b, int l, int a):fn(b), lvl(l), ass(a){}

    bool lt(Bop& oth)
    {return lvl < oth.lvl + oth.ass;}
};

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <iostream>

using namespace std;

typedef unordered_map<string, Uop*> Umap;
typedef unordered_map<string, Bop*> Bmap;

#define ERR(x) cout << x  << endl, exit (EXIT_FAILURE)

struct ParseErr : public runtime_error {
    ParseErr(string s) : runtime_error("Parse Error: "+s){}
};

struct Rand {
    int r;
    int val;

    Rand(int v):r(true), val(v){} //For 'constant' operators
    Rand():r(false){} //For all other operators

    virtual int ready() {return r;}
};

struct Var : public Rand {
    Var(string n):name(n){}
    string name;

    //rely on base class' ready and load
};

struct Monop : public Rand {
//For rators with 1 operand
    Rand* slotA;
    Uop* op;

    Monop(Uop *fn):op(fn){}

    virtual void load(Rand *r)
    {slotA = r;}

    virtual int ready();
};

struct Binop : public Rand {
//For rators with 2 operands
    Rand *slotA, *slotB;
    Bop* op;

    Binop(Bop *fn, Rand *a): op(fn), slotA(a){}
    
    Binop(Bop *fn): op(fn){}

    void init(Rand* r)
    {slotA = r;}
    
    virtual void load(Rand *r)
    {slotB = r;}

    virtual int ready();

    bool lt(Bop* b)
    {return op->lt(*b);}
};


struct Base : public Binop {
    Base(): Binop(new Bop(0,0,0)){}

    virtual int ready();
};
