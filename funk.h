/*------------Unary------------------*/
int abs(int a) {return a>0?a:-a;}

int neg(int a) {return -a;}

Umap umap({{"+", abs}, {"-", neg}});

/*------------Binary-----------------*/
int add(int a, int b) {return a+b;}

int sub(int a, int b) {return a-b;}

int mult(int a, int b) {return a*b;}

int divv(int a, int b) {return a/b;}

Bmap bmap({{"+", new BopO(add,1,0)}, {"-", new BopO(sub,1,0)}, {"*", new BopO(mult,2,0)}, {"/", new BopO(divv,2,0)}});
