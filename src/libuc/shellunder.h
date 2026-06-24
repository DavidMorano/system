/* shellunder HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* parse a c-string into the shell-under information components */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SHELLUNDER_INCLUDE
#define	SHELLUNDER_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* system-types */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	SHELLUNDER_DAT	struct shellunder_data


struct shellunder_data {
	cchar		*execname ;	/* child program exec-name */
	pid_t		pid ;		/* parent (shell) PID */
} ; /* end struct (shellunder_data) */

typedef	SHELLUNDER_DAT	shellunder_dat ;

EXTERNC_begin

extern int	shellunder_load(shellunder_dat *,cchar *) noex ;

local inline int shellunder_wr(shellunder_dat *sup,cchar *sp) noex {
	return shellunder_load(sup,sp) ;
}

EXTERNC_end


#endif /* SHELLUNDER_INCLUDE */


