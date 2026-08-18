/* calent HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* calendar entry object (for CALYEARS) */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	CALENT_INCLUDE
#define	CALENT_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<calcite.h>		/* LIBDAM */


#define	CALENT		struct calent_head
#define	CALENT_LN	struct calent_line
#define	CALENT_FL	struct calent_flags
#define	CALENT_Q	calcite
#define	CALENT_NLE	4		/* starting number of lines */
#define	CALENT_MAGIC	0x99000001


struct calent_line {
	uint		loff ;
	int		llen ;
} ; /* end struct */

struct calent_flags {
	uint		hash:1 ;
} ; /* end struct */

struct calent_head {
	CALENT_LN	*lines ;
	CALENT_Q	cite ;
	CALENT_FL	fl ;
	uint		magval ;
	uint		voff ;
	uint		vlen ;
	uint		hash ;
	int		e ;		/* (lines) extent */
	int		i ;		/* (lines) index */
	int		cidx ;
} ; /* end struct */

typedef	CALENT		calent ;
typedef	CALENT_FL	calent_fl ;
typedef	CALENT_LN	calent_ln ;
typedef	CALENT_Q	calent_q ;

EXTERNC_begin

extern int	calent_start	(calent *,con calent_q *,uint,int) noex ;
extern int	calent_setidx	(calent *,int) noex ;
extern int	calent_add	(calent *,uint,int) noex ;
extern int	calent_finish	(calent *) noex ;
extern int	calent_getci	(calent *) noex ;
extern int	calent_getq	(calent *,mut calent_q *) noex ;
extern int	calent_mkhash	(calent *,cchar *) noex ;
extern int	calent_sethash	(calent *,uint) noex ;
extern int	calent_gethash	(calent *,uint *) noex ;
extern int	calent_loadbuf	(calent *,char *,int,cchar *) noex ;

extern int	cmpcalent(con calent *,con calent *) noex ;

EXTERNC_end


#endif /* CALENT_INCLUDE */


