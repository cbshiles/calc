#include <iostream> //cout
#include <fstream> //ifstream
#include <sstream> //istringstream
#include "op.hpp"
#include "funk.hpp"

istream& read_file(const char* fname){
    ifstream* rFile = new ifstream;
    rFile->open(fname);

    if (! rFile->is_open())
	ERR("Error opening file: " << fname);

    return *rFile;
}

istream& read_string(const char* s)
{
    return *(new istringstream(string(s)));
}

Binop* chunk(istream& strm, Binop* src){
    string str;
    Rand *root=0; //root of current level
    Monop *tip;
    bool snd; //stream not done
    Uop* u;

    while ((snd = strm >> str) && (u = umap[str])){//Unary op
	Monop* mptr = new Monop(u);
	if (! root) root = tip = mptr;
	else {tip->load(mptr); tip = mptr;}
    }

    if (snd) {//str still has unprocessed token, "True" operand
	int v = stoi(str.c_str());
	if (!root) root = new Rand(v);
	else tip->load(new Rand(v));
    }
    else throw ParseErr("Expression missing operand.");    

    if (!(strm >> str)) {src->load(root); return 0;} //Done, bubble up
    else { //Look up binary operator
	Bop *b = bmap[str];
	
	if (!b) throw ParseErr("Operator not identified: " + str);

	if (src->lt(b)){//Making recursive call
	    Binop *bp, *rmp = new Binop(b, root);
	    while((bp = chunk(strm, rmp)) && src->lt(bp->op)){
		bp->init(rmp);
		rmp = bp;
	    }

	    src->load(rmp);
	    return bp;
	    
	} else { //Returning
	    src->load(root);
	    return new Binop(b);
	}
    }
}

int calc(istream& strm)
{
    Base base;
    if (chunk(strm, &base))
	throw ParseErr("Top level chunk returning value");
    base.ready();
    delete &strm;
    return base.val;
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

    test(9);
    
    testF("2 + 3 - /", 2);
    testF("2 + 3 -", 2);
    testF("2 & 7", 4); 
//    is = read_file("source.txt");
}
