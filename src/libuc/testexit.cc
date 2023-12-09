/* testexit */

/****

Compile and execute:
$ gpp -p testexit.x testexot.cc testsub.cc

****/


#include	<envstandards.h>
#include	<cstdlib>
#include	<cstring>
#include	<dlfcn.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<iostream>

using namespace std ;

extern void	sub() noex ;

extern "C" {
    static void efunc() noex ;
}

struct obj {
	obj() noex {
	    cchar	*sp = "main.ctor\n" ;
	    cint	sl = strlen(sp) ;
	    write(1,sp,sl) ;
	} ;
	~obj() noex {
	    cchar	*sp = "main.dtor\n" ;
	    cint	sl = strlen(sp) ;
	    write(1,sp,sl) ;
	} ;
} ;

static const obj	obj_data ;

int main(int,const char **,const char **) {
	cout << "main.begin\n" ;
	{
	    atexit(efunc) ;
	    sub() ;
	}
	cout << "main.end\n" ;
	exit(0) ;
}

static void efunc() noex {
	    cchar	*sp = "main.efunc\n" ;
	    cint	sl = strlen(sp) ;
	    write(1,sp,sl) ;
}


