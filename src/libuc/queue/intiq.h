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


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<fifoitem.h>		/* LIBUC */


#define	INTIQ		struct intiq_head
#define	INTIQ_MAGIC	0x23043953


struct intiq_head {
	ptm		*mxp ;
	fifoitem	*fqp ;
	uint		magval ;
} ; /* end struct (intiq_head) */

#ifdef	__cplusplus
enum intiqmems {
	intiqmem_start,
	intiqmem_count,
	intiqmem_finish,
	intiqmem_overlast
} ; /* end enum (intiqmems) */
struct intiq ;
struct intiq_co {
        intiq		*op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (intiq *p,int m) noex {
            op = p ;
            w = m ;
        } ;
        operator int () noex ;
} ; /* end struct (intiq_co) */
struct intiq : intiq_head {
	intiq_co	start ;
	intiq_co	count ;
	intiq_co	finish ;
	constexpr intiq() noex {
	    start	(this,intiqmem_start) ;
	    count	(this,intiqmem_count) ;
	    finish	(this,intiqmem_finish) ;
	    magval = 0 ;
	} ; /* end ctor */
	intiq(const intiq &) = delete ;
	intiq &operator = (const intiq &) = delete ;
	int ins		(int) noex ;
	int rem		(int *) noex ;
	void dtor	() noex ;
	operator int () noex ;
	destruct intiq() {
	    if (magval) dtor() ;
	} ; /* end dtor (intiq) */
} ; /* end class (intiq) */
#else
typedef INTIQ		intiq ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	intiq_start	(intiq *) noex ;
extern int	intiq_ins	(intiq *,int) noex ;
extern int	intiq_rem	(intiq *,int *) noex ;
extern int	intiq_count	(intiq *) noex ;
extern int	intiq_finish	(intiq *) noex ;

EXTERNC_end


#endif /* INTIQ_INCLUDE */


