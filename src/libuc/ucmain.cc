/* ucmain SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Q. What is this? You ask?
	A. This little "arrangement" in the code below is for those
	systems that use the GNU C-Language library (and libraries
	like it) that do not actually call |exit(3c)| when |main()|
	returns.  This beahvior is coded within one of the
	Common-Run_Time code pieces (or trampolines) that are
	supplied by the library.  Rather those libraries just fall
	into the "exit" handling code without actually calling
	|exit(3)| as a subroutine.  This (brain-damaged) arrangement
	by these (and possibly other) libraries do not allow for
	user code to intercept |exit(3)| when they want to do that
	(for their own good private reasons). So we adhere to this
	convention below (using |mainprog()| as a substitute |main()|
	is order to be able to intercept |exit(?)| when the new
	|main()| (now |mainprog()|) exits.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdlib>
#include	<utility>
#include	<usystem.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<exitcodes.h>


/* local defines */


/* external subroutines */

extern "C" {
    extern int	mainprog(int,const char **,const cchar **) noex ;
}


/* forward references */


/* exported subroutines */

int main(int argc,char **argv,char **envv) {
	int	ex ;
	try {
	    ex = mainprog(argc,argv,envv) ;
	} catch (...) {
	    ex = EX_TERM ;
	}
	exit(ex) ;
	std::unreachable() ;
}
/* end subroutine (main) */


