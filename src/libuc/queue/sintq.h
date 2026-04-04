/* sintq HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* short-integer queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This obejct module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	sintq

	Description:
	This module implements a short-intger queue object.

*******************************************************************************/

#ifndef	SINTQ_INCLUDE
#define	SINTQ_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


#define	SINTQ		struct sintq_head
#define	SINTQ_DEFENTS	10


struct sintq_head {
	short		*qbuf ;
	int		qlen ;
	int		ri ;
	int		wi ;
	int		cnt ;
} ; /* end struct (sintq_head) */

#ifdef	__cplusplus
enum sintqmems {
	sintqmem_start,
	sintqmem_remall,
	sintqmem_size,
	sintqmem_count,
	sintqmem_len,
	sintqmem_finish,
	sintqmem_overlast
} ; /* end enum (sintqmems) */
struct sintq ;
struct sintq_co {
        sintq		*op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (sintq *p,int m) noex {
            op = p ;
            w = m ;
        } ;
	int operator () (int = 0) noex ;
        operator int () noex {
	    return operator () (0) ;
	} ;
} ; /* end struct (sintq_co) */
struct sintq : sintq_head {
	sintq_co	start ;
	sintq_co	remall ;
	sintq_co	size ;
	sintq_co	count ;
	sintq_co	len ;
	sintq_co	finish ;
	constexpr sintq() noex {
	    start	(this,sintqmem_start) ;
	    remall	(this,sintqmem_remall) ;
	    size	(this,sintqmem_size) ;
	    count	(this,sintqmem_count) ;
	    len		(this,sintqmem_len) ;
	    finish	(this,sintqmem_finish) ;
	    qbuf = nullptr ;
	} ; /* end ctor */
	sintq(const sintq &) = delete ;
	sintq &operator = (const sintq &) = delete ;
	int	ins(int) noex ;
	int	rem(short *) noex ;
	int	remread(short *,int) noex ;
	void	dtor() noex ;
	operator int () noex ;
	destruct sintq() {
	    if (qbuf) dtor() ;
	} ; /* end dtor (sintq) */
} ; /* end class (sintq) */
#else
typedef SINTQ		sintq ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	sintq_start	(sintq *,int) noex ;
extern int	sintq_ins	(sintq *,int) noex ;
extern int	sintq_rem	(sintq *,short *) noex ;
extern int	sintq_remread	(sintq *,short *,int) noex ;
extern int	sintq_remall	(sintq *) noex ;
extern int	sintq_size	(sintq *) noex ;
extern int	sintq_count	(sintq *) noex ;
extern int	sintq_finish	(sintq *) noex ;

local inline int sintq_len(sintq *op) noex {
    	return sintq_count(op) ;
}

EXTERNC_end


#endif /* SINTQ_INCLUDE */


