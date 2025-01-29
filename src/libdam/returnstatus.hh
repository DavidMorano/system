/* return-status HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	RETURNSTATS_INCLUDE
#define	RETURNSTATS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


class returnstatus {
	int		rs ;
public:
	returnstatus(int a) : rs(a) { } ;
	int get() const { 
	    return rs ; 
	} ;
	void set(int a) { 
	    rs = a ; 
	} ;
} ; /* end struct (returnstatus) */


#endif /* RETURNSTATS_INCLUDE */


