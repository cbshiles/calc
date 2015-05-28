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
	    try { //Is a constant
		int x = stoi(str.c_str());
		tmp = new Rand(x);
	    }
	    catch(const invalid_argument& ia) { //Is a var
		ERR("Invalid " << ia.what() << " on " << str);
		//f = new Var(str);
	    }
	    if (! root) root = tmp;
	    else tip->load(tmp);
	    valid = 1; //True operand, good to go
	}
    }

    if(! valid) ERR("Could not completely load operand, stopped at " << str);
    else if(strm->eof()) {src->load(root); return 0;} //Dond dond
    else { //Look up binary operator
	BopO* b = bmap[str];
	if (b) { //Youve got a binary op

	    if (b->lvl > lvl) { //Staying inside
		Binop *rmp = new Binop(b, root);
		while (1) {
		    Binop *bp = chunk(strm, rmp->lvl, rmp);

		    if (! bp) {
			src->load(rmp);
			return 0;
		    }

		    else if (bp->lvl >= lvl) { //re-root this funk level
			bp->init(rmp);
			rmp = bp;
		    }

		    else {
			src->load(rmp); 
			return bp;			    
		    }
		    
		//     else if (bp->lvl < b->lvl){ //Move on, send up

		// 	if (lvl < bp->lvl){ //this means the parent of this one has lower priority than the one bubbling up
		// 	    bp->init(rmp);
		// 	    return bp;
		// 	    //src->load(bp);
		// 	} else {
		// 	    src->load(rmp); //associates with old op
		// 	    return bp;			    
		// 	}
		//     }

		//     else {
		// 	bp->init(rmp); //associates with new op
		// 	rmp = bp;
		//     }
		// }
		    
	    } else { //Going on top
		src->load(root);
		return new Binop(b);
	    }
	}
	else 
	    ERR("Operator not identified: " << str);
	    
    }
}
// int calc(istream* strm){

//     Rand *curr=0, *stab=0;

//     string str;
//     int state=0;
//     while(! strm->eof()){
// 	(*strm) >> str; // <<

// 	if (state) { //Looking for op2's

// 	    BopO* b = bmap[str];
// 	    if (b) ;//Youve got a binary op
// 	    else 
// 		ERR("Operator not identified: " << str);
// 	    state = 0;
// 	    curr = 0;
// 	} 

// 	else {  //Looking for op1 & rands

// 	    Uop u = umap[str];
// 	    Rand *f;
	    
// 	    if (u) {//Youve got a unary op
// 		f = new Monop(u);
// 		if (! curr) root = curr = f;
// 		else {curr->load(f); curr = curr->slotA;}
// 	    }

// 	    else {

// 		try { // Is a num
// 		    int x = stoi(str.c_str());
// 		    f = new Rand(x);
// 		}
// 		catch(const invalid_argument& ia) { //Is a var
// 		    ERR("Invalid " << ia.what() << " on " << str);
// 		    //f = new Var(str);
// 		}
		
// 		if (! curr) curr = f;
// 		else curr->load(f);
// 		state = 1;
// 	    }
	    
// 	}
//     }

//     delete strm;
//     return 5;
// }

int main()
{
    istream* is;
    is = read_string("4 * 2 / 3 - 1 + 5 < 6 - 7 + z");
//    is = read_file("source.txt");
//    calc(is);
}
