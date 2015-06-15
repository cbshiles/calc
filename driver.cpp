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

Base* rcalc(istream& strm);

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
	Rand* operand;
	if (str.compare("(") == 0) operand = rcalc(strm);
	else {
	    int v = stoi(str.c_str());
	    operand = new Rand(v);
	}
	
	if (!root) root = operand;
	else tip->load(operand);
    }
    else throw ParseErr("Expression missing operand.");    

    if (!(strm >> str) || str.compare(")") == 0) {src->load(root); return 0;} //Done, bubble up
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

Base* rcalc(istream& strm)
{
    Base* b = new Base;

    if (chunk(strm, b))
	throw ParseErr("Top level chunk returning value");

    return b;
}

int calc(istream& strm)
{
    Base* base = rcalc(strm);

    if (! strm.eof())
	throw ParseErr("Entire expression was not read, probably due to an unmatched right paren ))");
    
    base->ready();
    delete &strm;
    return base->val;
}

int testF(const char* expr, int ans)
{
    try {
	int got = calc(read_string(expr));
	if (got != ans){
	    cout << "Problem: " << expr <<
		" C says " << ans << ", we say " << got << endl;
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

    test( ( 2 ) + ( 2 ) );

    test( 2 * ( 8 + 5 * 6 + 2 ) + 3 + ( - - 4 * 2 ) );
    testF("2 + ( 3 - 4", 2 + ( 3 - 4 ));
    
    /* ERRORS */
    testF("2 + 3 - /", 2);
    testF("2 + 3 -", 2);
    testF("2 & 7", 4); 
    testF("2 + 7 ) + 4", 1);

//    is = read_file("source.txt");
}
