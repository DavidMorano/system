/* libtest SUPPORT */
/* lang=C++20 */

/* test library */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstdlib>
#include	<cstring>
#include	<utility>
#include	<dlfcn.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<iostream>


/* local defines */


/* local namespaces */

using namespace std ;


/* external subroutines */

extern "C" {
    extern void sub() noex ;
    extern void exit(int) noex ;
}

extern "C" {
    static void efunc() noex ;
}

namespace {
    struct libobj {
	libobj() noex {
	    cchar	*sp = "libtest.ctor\n" ;
	    cint	sl = strlen(sp) ;
	    write(1,sp,sl) ;
	} ;
	~libobj() noex {
	    cchar	*sp = "libtest.dtor\n" ;
	    cint	sl = strlen(sp) ;
	    write(1,sp,sl) ;
	} ;
    } ;
}

static const libobj	obj_data ;

void sub() noex {
	    cchar	*sp = "libtest.sub\n" ;
	    cint	sl = strlen(sp) ;
	    write(1,sp,sl) ;
	atexit(efunc) ;
}

void exit(int ex) {
	    cchar	*sp = "libtest.exit\n" ;
	    cint	sl = strlen(sp) ;
	    write(1,sp,sl) ;
	{
	    void	*dlp = dlsym(RTLD_NEXT,"exit") ;
	    typedef void (*void_f)(int) ;
	    void_f	ef = (void_f) dlp ;
	    (*ef)(ex) ;
	}
	std::unreachable() ;
}

static void efunc() noex {
	    cchar	*sp = "libtest.efunc\n" ;
	    cint	sl = strlen(sp) ;
	    write(1,sp,sl) ;
}


