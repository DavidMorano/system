/* teststrnul */
/* lang=C++98 */

/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<cstdlib>
#include	<cstdio>
#include	<iostream>
#include	<strnul.hh>


using std::cout ;

typedef const char		cchar ;
typedef const char *const 	*mainv ;

int main(int argc,mainv,mainv) {
	int		ex = 0 ;
	cchar		*sp1 = "Hello world!" ;
	cchar		*sp2 = "Muck raker!" ;
	{
	    strnul	s(sp1,20) ;
	    cchar	*rp ;
	    rp = s ;
	    cout << rp << '\n' ;
	    s(sp2,7) ;
	    rp = s ;
	    cout << rp << '\n' ;
	}
	return ex ;
}
/* end subroutine (main) */


