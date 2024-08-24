/* teststrnul SUPPORT */
/* lang=C++98 */

/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstdlib>
#include	<cstdio>
#include	<string>
#include	<string_view>
#include	<iostream>
#include	<strnul.hh>


using std::cout ;
using std::string_view ;

typedef std::string_view	strview ;
typedef const char *const 	*mainv ;

int main(int,mainv,mainv) {
	int		ex = 0 ;
	constexpr cchar	sp1[] = "Hello world!" ;
	constexpr cchar	sp2[] = "Muck raker!" ;
	{
	    strnul	s(sp1,11) ;
	    cchar	*rp ;
	    cout << sp1 << eol ;
	    rp = s ;
	    cout << rp << eol ;
	    s(sp2,7) ;
	    rp = s ;
	    cout << rp << eol ;
	    rp = s(sp1,3) ;
	    cout << rp << eol ;
	    cout << s(sp1,9) << eol ;
	    cout << sp2 << eol ;
	    cout << s(sp2,8) << eol ;
	    {
		strview	sv(sp2,10) ;
	        cout << sv << eol ;
	    }
	}
	return ex ;
}
/* end subroutine (main) */


