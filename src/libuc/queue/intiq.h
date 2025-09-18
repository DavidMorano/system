/* intiq HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Integer-Interlocked Queue */
/* version %I% last-modified %G% */


/* revision history:

	= 2017-11-24, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	intiq

	Description:
	Manage interlocked FIFO-integer operations.

*******************************************************************************/

#ifndef	INTIQ_INCLUDE
#define	INTIQ_INCLUDE


#include	<envstandards.h>	/* must be before others */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ptm.h>
#include	<fifoitem.h>


#define	INTIQ		struct intiq_head
#define	INTIQ_MAGIC	0x23043953


struct intiq_head {
	ptm		*mxp ;
	fifoitem	*fqp ;
	uint		magic ;
} ;

typedef INTIQ		intiq ;

EXTERNC_begin

extern int	intiq_start	(intiq *) noex ;
extern int	intiq_ins	(intiq *,int) noex ;
extern int	intiq_rem	(intiq *,int *) noex ;
extern int	intiq_count	(intiq *) noex ;
extern int	intiq_finish	(intiq *) noex ;

EXTERNC_end


#endif /* INTIQ_INCLUDE */


