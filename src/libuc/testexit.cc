/* testexit SUPPORT */
/* lang=C++20 */

/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/****

Compile and execute:
$ gpp -o testexit.x testexit.cc

****/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstdlib>
#include	<cstring>
#include	<dlfcn.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<iostream>

using namespace std ;

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
	}
	cout << "main.end\n" ;
	exit(0) ;
}

static void efunc() noex {
	cchar	*sp = "main.efunc\n" ;
	cint	sl = strlen(sp) ;
	write(1,sp,sl) ;
}


