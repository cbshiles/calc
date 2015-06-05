#include <cmath>

/*------------Unary------------------*/
int abs(int a) {return a>0?a:-a;}

int neg(int a) {return -a;}

Umap umap({{"+", new Uop(abs)}, {"-", new Uop(neg)}});

/*------------Binary-----------------*/
int add(int a, int b) {return a+b;}

int sub(int a, int b) {return a-b;}

int mult(int a, int b) {return a*b;}

int divv(int a, int b) {return a/b;}

int expt(int b, int p) {return (int) pow(b,p)+.1;}

Bmap bmap({{"+", new Bop(add,1,0)}, {"-", new Bop(sub,1,0)}, {"*", new Bop(mult,2,0)}, {"/", new Bop(divv,2,0)}});
