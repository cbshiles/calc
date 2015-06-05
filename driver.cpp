#include <iostream> //cout
#include <fstream> //ifstream
#include <sstream> //istringstream
#include "op.hpp"
#include "funk.hpp"

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
    string str;
    Rand *root=0;
    Monop *tip;
    bool valid = 0;
    
    while (*strm >> str && !valid){
	Uop* u = umap[str];

	if(u)  { //Unary op
	    Monop* mptr = new Monop(u);
	    if (! root) root = tip = mptr;
	    else {tip->load(mptr); tip = mptr;}
	}
	else { // "True" operand
	    int v = stoi(str.c_str());
	    if (!root) root = new Rand(v);
	    else tip->load(new Rand(v));
	    valid = 1; //Now expecting Binop
	}
    }
    if(! valid) throw ParseErr("Expression missing operand.");
    else if (!(*strm >> str)) {src->load(root); return 0;} //Done, bubble up
    else { }//Look up binary operator
}

int calc(istream* strm)
{
    Base base;
    if (chunk(strm, 0, &base))
	throw ParseErr("Top level chunk returning value");
    base.ready();
    delete strm;
    return base.val;
}

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

    test(9);
    
    testF("2 + 3 - /", 2);
    testF("2 + 3 -", 2);
    testF("2 & 7", 4); 
//    is = read_file("source.txt");
}
