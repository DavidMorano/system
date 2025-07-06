/* paramopt HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* paramater option manipulations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PARAMOPT_INCLUDE
#define	PARAMOPT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	PARAMOPT	struct paramopt_head
#define	PARAMOPT_NAME	struct paramopt_namer
#define	PARAMOPT_VAL	struct paramopt_value
#define	PARAMOPT_CUR	struct paramopt_cursor
#define	PARAMOPT_MAGIC	0x87892334


struct paramopt_value {
	PARAMOPT_VAL	*next ;
	cchar		*value ;
} ;

struct paramopt_namer {
	cchar		*name ;		/* key */
	PARAMOPT_NAME	*next ;
	PARAMOPT_VAL	*head ;
	PARAMOPT_VAL	*tail ;
	PARAMOPT_VAL	*current ;	/* used for interation only */
	int		c ;		/* count of values */
} ;

struct paramopt_head {
	PARAMOPT_NAME	*head ;
	PARAMOPT_NAME	*tail ;
	uint		magic ;
	int		f_inited ;
} ;

struct paramopt_cursor {
	PARAMOPT_NAME	*keyp ;
	PARAMOPT_VAL	*valp ;
} ;

typedef PARAMOPT_NAME	paramopt_name ;
typedef PARAMOPT_VAL	paramopt_val ;
typedef PARAMOPT_CUR	paramopt_cur ;

#ifdef	__cplusplus
enum paramoptmems {
	paramoptmem_start,
	paramoptmem_count,
	paramoptmem_incr,
	paramoptmem_finish,
	paramoptmem_overlast
} ;
struct paramopt ;
struct paramopt_co {
	paramopt	*op = nullptr ;
	int		w = -1 ;
	void operator () (paramopt *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (paramopt_co) */
struct paramopt : paramopt_head {
	paramopt_co	start ;
	paramopt_co	count ;
	paramopt_co	incr ;
	paramopt_co	finish ;
	paramopt() noex {
	    start(this,paramoptmem_start) ;
	    count(this,paramoptmem_count) ;
	    incr(this,paramoptmem_incr) ;
	    finish(this,paramoptmem_finish) ;
	} ;
	paramopt(const paramopt &) = delete ;
	paramopt &operator = (const paramopt &) = delete ;
	int loadu(cchar *,int) noex ;
	int loads(cchar *,cchar *,int) noex ;
	int load(cchar *,cchar *,int) noex ;
	int loaduniq(cchar *,cchar *,int) noex ;
	int loadone(cchar *,int) noex ;
	int havekey(cchar *) noex ;
	int haveval(cchar *,cchar *,int) noex ;
	int countvals(cchar *) noex ;
	int curbegin(paramopt_cur *) noex ;
	int curend(paramopt_cur *) noex ;
	int curenumkey(paramopt_cur *,cchar **) noex ;
	int curenumval(cchar *,paramopt_cur *,cc **) noex ;
	int curfetch(cchar *,paramopt_cur *,cchar **) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct paramopt() {
	    if (head) dtor() ;
	} ;
} ; /* end struct (paramopt) */
#else	/* __cplusplus */
typedef PARAMOPT	paramopt ;
#endif /* __cplusplus */

EXTERNC_begin

extern int paramopt_start(paramopt *) noex ;
extern int paramopt_loadu(paramopt *,cchar *,int) noex ;
extern int paramopt_loads(paramopt *,cchar *,cchar *,int) noex ;
extern int paramopt_load(paramopt *,cchar *,cchar *,int) noex ;
extern int paramopt_loaduniq(paramopt *,cchar *,cchar *,int) noex ;
extern int paramopt_loadone(paramopt *,cchar *,int) noex ;
extern int paramopt_havekey(paramopt *,cchar *) noex ;
extern int paramopt_haveval(paramopt *,cchar *,cchar *,int) noex ;
extern int paramopt_count(paramopt *) noex ;
extern int paramopt_countvals(paramopt *,cchar *) noex ;
extern int paramopt_incr(paramopt *) noex ;
extern int paramopt_curbegin(paramopt *,paramopt_cur *) noex ;
extern int paramopt_curend(paramopt *,paramopt_cur *) noex ;
extern int paramopt_curenumkey(paramopt *,paramopt_cur *,cchar **) noex ;
extern int paramopt_curenumval(paramopt *,cchar *,paramopt_cur *,cc **) noex ;
extern int paramopt_curfetch(paramopt *,cchar *,paramopt_cur *,cchar **) noex ;
extern int paramopt_finish(paramopt *) noex ;

EXTERNC_end

#ifdef	__cplusplus

template<typename ... Args>
static inline int paramopt_magic(paramopt *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == PARAMOPT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (paramopt_magic) */

#endif /* __cplusplus */


#endif /* PARAMOPT_INCLUDE */


