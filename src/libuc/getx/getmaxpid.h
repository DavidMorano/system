/* getseed HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* get the maximum PID that the current system supports */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	Copied from |gethz(3dam)|.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getmaxpid

	Synopsis:
	int getmaxpid(int w) noex 

	Arguments:
	w		which maximum to report:
				0	all processes
				1	system processes (where available)

	Returns:
	>0		MAXPID value
	==0		?
	<0		error (system-return)

	Notes:
	+ We try to get a dynamic value if we can, otherwise we let
	the system itself guess (w/ PID_MAX), thereafter we guess
	with our own value.  There is no dynamic value (that we know
	of) for the maximum "system" PID.

*******************************************************************************/

#ifndef	GETMAXPID_INCLUDE
#define	GETMAXPID_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	getmaxpid(int) noex ;

EXTERNC_end

#ifdef	__cplusplus

namespace libuc {
    struct maxpider {
    	operator int () noex {
    	    return getmaxpid(0) ;
	} ;
	int operator () () noex {
	    return operator int () ;
	} ;
    } ; /* end struct (maxpider) */
}

extern libuc::maxpider		maxpid ;

#endif /* __cplusplus */


#endif /* GETMAXPID_INCLUDE */


