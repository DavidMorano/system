/* procse HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* expand server entry */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PROCSE_INCLUDE
#define	PROCSE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<varsub.h>		/* LIBUC */
#include	<expcook.h>		/* LIBUC */


#define	PROCSE		struct procse_head
#define	PROCSE_ARGS	struct procse_arguments


struct procse_arguments {
	cchar		*passfile ;	/* pass-file */
	cchar		*sharedobj ;	/* shared-object path */
	cchar		*program ;	/* server program path */
	cchar		*srvargs ;	/* server program arguments */
	cchar		*username ;
	cchar		*groupname ;
	cchar		*options ;
	cchar		*access ;
	cchar		*failcont ;
} ; /* end struct */

struct procse_head {
	mainv		envv ;
	varsub		*vsp ;
	PROCSE_ARGS	*ap ;
	PROCSE_ARGS	a ;
} ; /* end struct */

typedef	PROCSE		procse ;
typedef	PROCSE_ARGS	procse_args ;

EXTERNC_begin

extern int procse_start(procse *,con mainv,varsub *,procse_args *) noex ;
extern int procse_process(procse *,expcook *) noex ;
extern int procse_finish(procse *) noex ;

EXTERNC_end


#endif /* PROCSE_INCLUDE */


