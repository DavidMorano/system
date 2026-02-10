/* uids HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* LIBU process ID operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10, David A­D­ Morano
	This code was originally written.  This subroutines
	(or something similar to it) is standard on some UNIXes but
	not on others, so it is now provided.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

 	Object:
	uids

	specified directory path if they do not exist already.

	Synopsis:

	Arguments:

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#ifndef	UIDS_INCLUDE
#define	UIDS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>


namespace libu {
    class uids ;
    typedef int (uids::*uids_m)() noex ;
    class uids {
	int iload() noex { 
	    return 0 ;
	} ;
	int irelease() noex { 
	    return 0 ;
	} ;
	struct uids_co {
	    uids	*op = nullptr ;
	    uids_m	m ;
	    void operator () (uids *o,uids_m am) noex {
		op = o ;
		m = am ;
	    } ;
	    operator int () noex {
		return (op->*m)() ;
	    } ;
	} ; /* end struct (ids_co) */
    public:
	uids_co		load ;
	uids_co		release	;
	const uid_t	euid = geteuid() ;
	const gid_t	egid = getegid() ;
	uids() noex {
	    load	(this,&uids::iload) ;
	    release	(this,&uids::irelease) ;
	} ; /* end ctor */
	int perm(ustat *,int) noex ;
    } ; /* end struct (uids) */
} /* end namespace (libu) */


#endif /* UIDS_INCLUDE */


