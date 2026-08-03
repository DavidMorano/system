/* chariq HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Character-Interlocked Queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1994 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	chariq

	Description:
	This object manages interlocked FIFO-character operations.

*******************************************************************************/

#ifndef	CHARIQ_INCLUDE
#define	CHARIQ_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<charq.h>		/* LIBUC */
#include	<ptm.h>			/* LIBU */


#define	CHARIQ		struct chariq_head


struct chariq_head {
	charq		*cqp ;
	ptm		*mxp ;
} ; /* end struct (chariq_head) */

#ifdef	__cplusplus
enum chariqmems {
	chariqmem_remall,
	chariqmem_size,
	chariqmem_count,
	chariqmem_len,
	chariqmem_finish,
	chariqmem_overlast
} ; /* end enum (chariqmems) */
struct chariq ;
struct chariq_co {
        chariq		*op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (chariq *p,int m) noex {
            op = p ;
            w = m ;
        } ;
        operator int () noex ;
} ; /* end struct (chariq_co) */
struct chariq : chariq_head {
	chariq_co	remall ;
	chariq_co	size ;
	chariq_co	count ;
	chariq_co	len ;
	chariq_co	finish ;
	constexpr chariq() noex {
	    remall	(this,chariqmem_remall) ;
	    size	(this,chariqmem_size) ;
	    count	(this,chariqmem_count) ;
	    len		(this,chariqmem_len) ;
	    finish	(this,chariqmem_finish) ;
	    cqp = nullptr ;
	} ; /* end ctor */
	chariq(const chariq &) = delete ;
	chariq &operator = (const chariq &) = delete ;
	int start	(int) noex ;
	int ins		(int) noex ;
	int rem		(char *) noex ;
	void dtor	() noex ;
	operator int () noex ;
	destruct chariq() {
	    if (cqp) dtor() ;
	} ; /* end dtor (chariq) */
} ; /* end class (chariq) */
#else
typedef CHARIQ		chariq ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	chariq_start	(chariq *,int) noex ;
extern int	chariq_ins	(chariq *,int) noex ;
extern int	chariq_rem	(chariq *,char *) noex ;
extern int	chariq_remall	(chariq *) noex ;
extern int	chariq_size	(chariq *) noex ;
extern int	chariq_count	(chariq *) noex ;
extern int	chariq_finish	(chariq *) noex ;

EXTERNC_end


#endif /* CHARIQ_INCLUDE */


