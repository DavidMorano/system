/* shortq HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* short-word queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This obejct module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SHORTQ_INCLUDE
#define	SHORTQ_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	SHORTQ		struct shortq_head


struct shortq_head {
    	void		*qvp ;		/* queue-value-pointer */
} ;

typedef SHORTQ		shortq ;

EXTERNC_begin

extern int	shortq_start(shortq *,int) noex ;
extern int	shortq_ins(shortq *,int) noex ;
extern int	shortq_rem(shortq *,char *) noex ;
extern int	shortq_remall(shortq *) noex ;
extern int	shortq_size(shortq *) noex ;
extern int	shortq_count(shortq *) noex ;
extern int	shortq_finish(shortq *) noex ;

local inline int shortq_len(shortq *op) noex {
    	return shortq_count(op) ;
}

EXTERNC_end


#endif /* SHORTQ_INCLUDE */


