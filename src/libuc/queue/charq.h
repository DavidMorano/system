/* charq HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* character queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This obejct module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	charq

	Description:
	This module implements a character queue object.

*******************************************************************************/

#ifndef	CHARQ_INCLUDE
#define	CHARQ_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	CHARQ		struct charq_head


struct charq_head {
	char		*qbuf ;
	int		qlen ;
	int		ri ;
	int		wi ;
	int		cnt ;
} ; /* end enum (charq_head) */

#ifdef	__cplusplus
enum charqmems {
	charqmem_start,
	charqmem_remall,
	charqmem_size,
	charqmem_count,
	charqmem_len,
	charqmem_finish,
	charqmem_overlast
} ; /* end enum (charqmems) */
struct charq ;
struct charq_co {
        charq		*op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (charq *p,int m) noex {
            op = p ;
            w = m ;
        } ;
	int operator () (int = 0) noex ;
        operator int () noex {
	    return operator () (0) ;
	} ;
} ; /* end struct (charq_co) */
struct charq : charq_head {
	charq_co	start ;
	charq_co	remall ;
	charq_co	size ;
	charq_co	count ;
	charq_co	len ;
	charq_co	finish ;
	constexpr charq() noex {
	    start	(this,charqmem_start) ;
	    remall	(this,charqmem_remall) ;
	    size	(this,charqmem_size) ;
	    count	(this,charqmem_count) ;
	    len		(this,charqmem_len) ;
	    finish	(this,charqmem_finish) ;
	    qbuf = nullptr ;
	} ; /* end ctor */
	charq(const charq &) = delete ;
	charq &operator = (const charq &) = delete ;
	int ins		(int) noex ;
	int rem		(char *) noex ;
	void dtor	() noex ;
	operator int () noex ;
	destruct charq() {
	    if (qbuf) dtor() ;
	} ; /* end dtor (charq) */
} ; /* end class (charq) */
#else
typedef CHARQ		charq ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	charq_start	(charq *,int) noex ;
extern int	charq_ins	(charq *,int) noex ;
extern int	charq_rem	(charq *,char *) noex ;
extern int	charq_remall	(charq *) noex ;
extern int	charq_size	(charq *) noex ;
extern int	charq_count	(charq *) noex ;
extern int	charq_finish	(charq *) noex ;

local inline int charq_len(charq *op) noex {
    	return charq_count(op) ;
}

EXTERNC_end


#endif /* CHARQ_INCLUDE */


