/* pcsunodes HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* revision history:

	= 2000-09-10, Dave Morano
	This program was originally written.

*/

/* Copyright © 2000 David A-D- Morano.  All rights reserved. */

#ifndef	PCSUNODES_INCLUDE
#define	PCSUNODES_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	PCSUNODES	struct pcsunodes_head
#define	PCSUNODES_CUR	struct pcsunodes_cursor
#define	PCSUNODES_MAGIC	0x99447245


struct pcsunodes_cursor {
	int		i ;
} ; /* end struct */

struct pcsunodes_head {
	cchar		**unodes ;
	uint		magval ;
	int		n ;
} ; /* end struct */

typedef	PCSUNODES	pcsunodes ;
typedef	PCSUNODES_CUR	pcsunodes_cur ;

EXTERNC_begin

extern int pcsunodes_start	(pcsunodes *,cchar *) noex ;
extern int pcsunodes_get	(pcsunodes *,int,cchar **) noex ;
extern int pcsunodes_mat	(pcsunodes *,cchar *,int) noex ;
extern int pcsunodes_curbegin	(pcsunodes *,pcsunodes_cur *) noex ;
extern int pcsunodes_curenum	(pcsunodes *,pcsunodes_cur *,char *,int) noex ;
extern int pcsunodes_curend	(pcsunodes *,pcsunodes_cur *) noex ;
extern int pcsunodes_audit	(pcsunodes *) noex ;
extern int pcsunodes_finish	(pcsunodes *) noex ;

EXTERNC_end


#endif /* PCSUNODES_INCLUDE */


