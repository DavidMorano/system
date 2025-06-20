/* objcallout SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Object-Callout helper object */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	objcallout

	Description:
	This is a helper object to facilitates call-outs for other
	objects.

*******************************************************************************/

#ifndef	INCLUDE_UOBJLOCK
#define	INCLUDE_UOBJLOCK
#ifdef	__cplusplus /* C++ only! */


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


template<typename T> struct objcallout {
        T		*op = nullptr ;
        int             w = -1 ;
        void operator () (T *p,int m) noex {
            op = p ;
            w = m ;
        } ;
        operator int () noex ;
        int operator () () noex {
            return operator int () ;
        } ;
} ; /* end struct-template (objcallout) */


#endif /* __cplusplus (C++ only) */
#endif /* INCLUDE_UOBJLOCK */


