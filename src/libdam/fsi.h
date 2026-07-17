/* fsi HEADER (FIFO-String-Interlocked) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* FIFO-String-Interlocked */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fsi

	Description:
	Manage interlocked string-FIFO operations.

*******************************************************************************/

#ifndef	FSI_INCLUDE
#define	FSI_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<fifostr.h>		/* LIBUC */


#define	FSI		struct fsi_head


struct fsi_head {
	ptm		*mxp ;
	fifostr		*qsp ;
} ; /* end struct */

typedef FSI		fsi ;

EXTERNC_begin

extern int	fsi_start	(fsi *) noex ;
extern int	fsi_add		(fsi *,cchar *,int) noex ;
extern int	fsi_remove	(fsi *,char *,int) noex ;
extern int	fsi_rem		(fsi *,char *,int) noex ;
extern int	fsi_count	(fsi *) noex ;
extern int	fsi_finish	(fsi *) noex ;

EXTERNC_end


#endif /* FSI_INCLUDE */


