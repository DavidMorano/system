/* testinit SUPPORT */
/* lang=C++98 */

/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstdlib>
#include	<cstdio>
#include	<iostream>
#include	<utypedefs.h>
#include	<clanguage.h>


using std::cout ;

typedef const char *const 	*mainv ;

struct multi {
	int	a ;
	int	b ;
	int	c ;
} ;

static int	mkterms() noexcept ;

static multi	aa = { 1 } ;

extern "C" {
    static void __attribute__ ((constructor)) init() noexcept {
	printf("init\n") ;
    }
}

int main(int argc,const char **argv,const char **envv) {
	static int	srs = mkterms() ;
	int		rs ;
	int		ex = 0 ;
	if ((rs = srs) >= 0) {
	    printf("aa.a=%08x aa.b=%08x\n",aa.a,aa.b) ;
	    {
		cchar	*pn = getprogname() ;
		cout << "pn=" << pn << '\n' ; 
	    } /* end block */
	} /* end if */
	if (rs < 0) ex = 1 ;
	return ex ;
}
/* end subroutine (main) */

extern "C" {
    static void __attribute__ ((destructor)) fini() noexcept {
	printf("fini\n") ;
    }
}

static int mkterms() noexcept {
    cout << "mkterms\n" ;
    return 0 ;
}

