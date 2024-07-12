/* procse HEADER */
/* lang=C20 */

/* expanded server entry */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PROCSE_INCLUDE
#define	PROCSE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<varsub.h>
#include	<expcook.h>


#define	PROCSE		struct procse_head
#define	PROCSE_ARGS	struct procse_a


struct procse_a {
	cchar		*passfile ;		/* pass-file */
	cchar		*sharedobj ;		/* shared-object path */
	cchar		*program ;		/* server program path */
	cchar		*srvargs ;		/* server program arguments */
	cchar		*username ;
	cchar		*groupname ;
	cchar		*options ;
	cchar		*access ;
	cchar		*failcont ;
} ;

struct procse_head {
	cchar		**envv ;
	varsub		*vsp ;
	PROCSE_ARGS	*ap ;
	PROCSE_ARGS	a ;
} ;

EXTERNC_begin

extern int procse_start(PROCSE *,cchar **,varsub *,PROCSE_ARGS *) noex ;
extern int procse_process(PROCSE *,EXPCOOK *) noex ;
extern int procse_finish(PROCSE *) noex ;

EXTERNC_end


#endif /* PROCSE_INCLUDE */


