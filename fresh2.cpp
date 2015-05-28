#include <stdio.h> //printf
#include <iostream> //cout
#include <fstream> //ifstream
#include <sstream> //istringstream
#include <string>
#include <stdexcept>// std::invalid_argument
#include <vector>
#include <stdlib.h>
#include <unordered_map>

using namespace std;

typedef int(*Uop)(int);
typedef unordered_map<string, Uop> Umap;

typedef int(*Bop)(int, int);
struct BopO{
    BopO(Bop b, int i, int a):ptr(b), lvl(i), ass(a){}
    Bop ptr;
    int lvl, ass;
};
typedef unordered_map<string, BopO*> Bmap;

#define ERR(x) cout << x  << endl, exit (EXIT_FAILURE)

struct ParseErr : public std::runtime_error {
    ParseErr(string s) : std::runtime_error("Parse Error: "+s){}
};

#include "funk.h"

struct Rand {
    Rand(int v):r(1), val(v){} //For 'constant' operators
    Rand():r(0){} //For all other operators (in the broadest sense)
    virtual int ready() {return r;}

    virtual void load(Rand* r){ERR("Attempting to load a non-operator operand.");}

    int r, val;
};

struct Var : public Rand {
    Var(string n):name(n){}
    string name;

    //rely on base class' ready and load
};

//Note on operator constructors:
//Unary ops precede the operand, binary are infix
//Thus, unary won't know its operand when created
//Binary will only know one of its operands

struct Monop : public Rand {
//For rators with 1 operand
    Monop(Uop fn):op(fn){}
    
    Rand* slotA;
    Uop op;

    virtual void load(Rand *r){
	slotA = r;
    }
    
    virtual int ready()
    {
	if(slotA->ready()){
	    val = op(slotA->val);
	    delete slotA;
	    return 1;
	} else return 0;
    }
};

struct Base : public Monop {
    Base(): Monop(0){}

    virtual int ready()
    {
	if(slotA->ready()){
	    val = slotA->val;
	    delete slotA;
	    return 1;
	} else return 0;
    }

};

struct Binop : public Rand {
//For rators with 2 operands
    Binop(BopO *fn, Rand *a):
	op(fn->ptr), lvl(fn->lvl), ass(fn->ass), slotA(a){}
    
    Binop(BopO *fn):
	op(fn->ptr), lvl(fn->lvl), ass(fn->ass){}

    Rand *slotA, *slotB;
    Bop op;
    int lvl, ass;

    void init(Rand* r){
	slotA = r;
    }
    
    virtual void load(Rand *r){
	slotB = r;
    }
    
    virtual int ready()
    {
	if(slotA->ready() + slotB->ready() == 2){
	    val = op(slotA->val, slotB->val);
	    delete slotA;
	    delete slotB;
	    return 1;
	} else return 0;
    }
};

istream* read_file(const char* fname){
    ifstream* rFile = new ifstream;
    rFile->open(fname);

    if (! rFile->is_open())
	ERR("Error opening file: " << fname);

    return rFile;
}

istream* read_string(const char* s)
{
    return new istringstream(string(s));
}

Binop* chunk(istream* strm, int lvl, Rand* src){
    Rand *root=0, *tmp;
    Monop *tip; 
    string str;
    int valid = 0;
    
    while(!(strm->eof() || valid)){
	(*strm) >> str;  
	Uop u = umap[str];

	if(u)  { //Unary op
	    Monop* mptr = new Monop(u);
	    if (! root) root = tip = mptr;
	    else {tip->load(mptr); tip = mptr;}
	}
	
	else { // "True" operand 

	    int x = stoi(str.c_str());
	    tmp = new Rand(x);

	    if (! root) root = tmp;
	    else tip->load(tmp);
	    valid = 1; //True operand, good to go
	}
    }

    if(! valid) throw ParseErr("Expression missing operand."); 
    else if(strm->eof()) {src->load(root); return 0;} //Done, bubble up
    else { //Look up binary operator
	(*strm) >> str;  
	BopO* b = bmap[str];

	if (!b) throw ParseErr("Operator not identified: " + str);

	if (b->lvl > lvl) { //Staying inside
	    Binop *rmp = new Binop(b, root);
	    while (1) {
		Binop *bp = chunk(strm, rmp->lvl, rmp);

		if (! bp || bp->lvl <= lvl) {
		    src->load(rmp);
		    return bp;
		}

		else { //re-root this funk level
		    bp->init(rmp);
		    rmp = bp;
		}
	    }
	}

	else { //Going on top
	    src->load(root);
	    return new Binop(b);
	}
    }
}

int calc(istream* strm)
{
    Base base; int i=0;
    Rand* r = chunk(strm, 0, &base);
    base.ready();
    i = base.val;
    delete strm;
    return i;
}

int testF(const char* expr, int ans)
{
    try {
	int got = calc(read_string(expr));
	if (got != ans){
	    cout << "Problem: " << expr <<
		" should equal " << ans << ", got " << got << endl;
	} else {
	    cout << "Check\n";
	}
    }
    catch (exception& e) { cerr << expr << endl << e.what() << endl; }
}

#define test(ss) testF(#ss, ss)

int main()
{
    test(- 2 + 2);
    
//    test(+ -2 + 2);
    testF("+ -2 + 2", abs(-2) + 2);

    test(3 * 8 + - -2);
    test(2 + 2);
    test(1 - - -6);
    test(4 * 2 / 3 - 1 + 5 * 6 - 7 + - -4);
    test(6 - 7 - 8 - 9);

    testF("2 + 3 - /", 2);
    testF("2 + 3 -", 2);
    testF("2 & 7", 4); 
//    is = read_file("source.txt");
}
