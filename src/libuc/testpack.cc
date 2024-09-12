/* testpack SUPPORT */
/* lang=C++20 */

/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<iostream>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<usysrets.h>
#include	<sncpyx.h>
#include	<sncpyxw.h>


using std::nullptr_t ;			/* type */
using std::cout ;			/* variable */

template<typename ... A>
int thingx(cchar *,int,A ... arg,int) noex {
    return 0 ;
}
int thingx(cchar *,int,int,ccp,ccp,int) noex {
    return 0 ;
}

template<typename ... Args>
static int magic(Args ... args) noex {
	return (args && ...) ;
}

template<typename ... Args>
static int thingw(char *dp,int dl,Args ... args,int other) noex {
	const nullptr_t	np{} ;
	cint		na = npack(Args) ;
	int		rs = SR_OK ;
	switch (na) {
	case 0:
	    rs = thingx(dp,dl,na,args ...,np,np,np,other) ;
	    break ;
	case 1:
	    rs = thingx(dp,dl,na,args ...,np,np,other) ;
	    break ;
	case 2:
	    rs = thingx(dp,dl,na,args ...,np,other) ;
	    break ;
	case 3:
	    rs = thingx(dp,dl,na,args ...,other) ;
	    break ;
	} /* end switch */
	return rs ;
}

/* exported subroutines */

int main(int,cchar **,cchar **) {
	int	ex = 0 ;

	{
	    int	v1 = 1 ;
	    int	v2 = 2 ;
	    ex = magic(v1,v2) ;
	    if (ex) {
	        cout << "yes\n" ;
	    } else {
	        cout << "no\n" ;
	    }
	}

	{
		constexpr int dlen = 100 ;
		cint	other = 12 ;
		cchar	*s1 = "Hello " ;
		cchar	*s2 = "world!\n" ;
		char	dbuf[dlen+1] ;
		sncpy(dbuf,dlen,s1,s2) ;
		sncpyw(dbuf,dlen,s1,s2,other) ;
	}
	return ex ;
}
/* end subroutine (main) */


