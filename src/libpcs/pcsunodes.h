/* pcsunodes HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSUNODES_INCLUDE
#define	PCSUNODES_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<clanguage.h>
#include	<usysbase.h>


#define	PCSUNODES	struct pcsunodes_head
#define	PCSUNODES_CUR	struct pcsunodes_cursor
#define	PCSUNODES_MAGIC	0x99447245


struct pcsunodes_cursor {
	int		i ;
} ; /* end struct */

struct pcsunodes_head {
	cchar		**unodes ;
	uint		magic ;
	int		n ;
} ; /* end struct */

typedef	PCSUNODES	pcsunodes ;
typedef	PCSUNODES_CUR	pcsunodes_cur ;

EXTERNC_begin

extern int pcsunodes_start(PCSUNODES *,cchar *) noex ;
extern int pcsunodes_get(PCSUNODES *,int,cchar **) noex ;
extern int pcsunodes_mat(PCSUNODES *,cchar *,int) noex ;
extern int pcsunodes_curbegin(PCSUNODES *,pcsunodes_cur *) noex ;
extern int pcsunodes_enum(PCSUNODES *,pcsunodes_cur *,char *,int) noex ;
extern int pcsunodes_curend(PCSUNODES *,pcsunodes_cur *) noex ;
extern int pcsunodes_audit(PCSUNODES *) noex ;
extern int pcsunodes_finish(PCSUNODES *) noex ;

EXTERNC_end


#endif /* PCSUNODES_INCLUDE */


