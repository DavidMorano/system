/* dialtab HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* get additional machine dialing information */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DIALTAB_INCLUDE
#define	DIALTAB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecobj.h>		/* LIBUC */


#define	DIALTAB		struct dialtab_head
#define	DIALTAB_ENT	struct dialtab_entry


struct dialtab_entry {
	cchar		*name ;
	cchar		*inet ;
	cchar		*uucp ;
	cchar		*username ;
	cchar		*password ;
	int		fi ;
} ; /* end struct */

struct dialtab_head {
	vecobj		*flp ;
	vecobj		*elp ;
	uint		magval ;
} ; /* end struct */

typedef	DIALTAB		dialtab ;
typedef	DIALTAB_ENT	dialtab_ent ;

EXTERNC_begin

extern int	dialtab_open(dialtab *,cchar *) noex ;
extern int	dialtab_fileadd(dialtab *,cchar *) noex ;
extern int	dialtab_check(dialtab *,cchar *) noex ;
extern int	dialtab_search(dialtab *,cchar *,dialtab_ent **) noex ;
extern int	dialtab_close(dialtab *) noex ;

EXTERNC_end


#endif /* DIALTAB_INCLUDE */


