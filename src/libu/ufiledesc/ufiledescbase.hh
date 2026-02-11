/* ufiledescbase HEADER (UNIX® file operations) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:

	Description:
	All of the UNIX® system calls that use (operate on) a
	file-descroptor.

*******************************************************************************/

#ifndef	UFILEDESCBASE_INCLUDE
#define	UFILEDESCBASE_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usupport.h>


namespace libu {
    struct ufiledescflags {
	uint		fclose:1 ;
	uint		fwrite:1 ;
	uint		fintr:1 ;	/* request interrupts to return */
	uint		fatcwd:1 ;	/* allow |AT_FDCWD| file-descriptor */
    } ; /* end struct (ufiledescflags) */
    struct ufiledescbase {
	cvoid		*wbuf ;
	void		*rbuf ;
	int		wlen ;
	int		rlen ;
	ufiledescflags	fdfl{} ;
	ufiledescbase() noex { } ;
	int operator () (int) noex ;
	virtual int callstd(int) noex = 0 ;
    } ; /* end struct (ufiledescbase) */
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* UFILEDESCBASE_INCLUDE */


