/* isobjspecial SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* determine if a shared-object handle is special or not */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

	= 2020-10-22, David A-D- Morano
	I tried to make the constant array of RTLD_{xx} values (see
	the code below) 'constexpr' but C++ does not allow it since
	it does not allow casting integers to pointers (even to
	pointers to void) in a 'constexpr' context.

*/

/* Copyright © 1998,2020 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	isobjspecial

	Description:
	This subroutine determines if a library file is a special
	object or not.

	Synopsis:
	int isobjspecial(void *sop) noex

	Arguments:
	sop		shared-object handle

	Returns:
	TRUE		shared-object is special
	FALSE		shared-object is regular

	Notes:
	1. MacOS (Darwin) and possibly other OSes do not have the
	handle of RTLD_PROBE.

	2. In the data array below (w/ variable name 'objs') I note
	that the constant-data array cannot be make 'constexpr'
	because f*ck-*ass C++ does not let integers to be cast to
	a pointer (even a pointer-to-void) in a 'onstexpr' variable
	definition.  So the array defiition has to remain simply
	'static void *const []'.  The 'RTLD_{xx}' preprocessor defines
	are defined by the system implementation -- not by me --
	so they cannot be changed (defined differently).  So I have
	to work with what is given to me.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<dlfcn.h>		/* |RTLD_{xxx}| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdfnames.h>
#include	<matstr.h>
#include	<localmisc.h>

#include	"ismisc.h"


/* local defines */

#ifndef	RTLD_PROBE
#define	RTLD_PROBE		voidp(1)	/* real vals are negative */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

/* <- cannot do 'constexpr' cast of integer to ptr (f*ck up of C++) */
static void	*const objs[] = { 
	RTLD_DEFAULT,			/* <- integer cast to pointer-to-void */
	RTLD_NEXT,			/* <- integer cast to pointer-to-void */
	RTLD_SELF,			/* <- integer cast to pointer-to-void */
	RTLD_PROBE,			/* <- integer cast to pointer-to-void */
	nullptr
} ; /* end array (objs) */


/* exported variables */


/* exported subroutines */

bool isobjspecial(void *sop) noex {
	bool		f = false ;
	for (int i = 0 ; objs[i] ; i += 1) {
	    f = (sop == objs[i]) ;
	    if (f) break ;
	} /* end if */
	return f ;
}
/* end subroutine (isobjspecial) */

bool isfnamespecial(cchar *fp,int fl) noex {
	return (matstr(stdfnames,fp,fl) >= 0) ;
}
/* end subroutine (isfnamespecial) */


