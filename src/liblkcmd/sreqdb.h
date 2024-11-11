/* sreqdb HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* version %I% last-modified %G% */
/* perform various functions on a job */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SREQDB_INCLUDE
#define	SREQDB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vechand.h>
#include	<intiq.h>

#include	"sreq.h"


#define	SREQDB		struct sreqdb_head
#define	SREQDB_MAGIC	0x75385212
#define	SREQDB_JOBIDLEN	SREQ_JOBIDLEN	/* same as LOGIDLEN? */


struct sreqdb_head {
	cchar		*tmpdname ;
	vechand		db ;
	INTIQ		exits ;		/* thread-exits by job-ID (jsn) */
	volatile int	f_threxiting ;	/* a child thread is exiting */
	uint		magic ;
} ;

EXTERNC_begin

extern int sreqdb_start(SREQDB *,cchar *,int) noex ;
extern int sreqdb_newjob(SREQDB *,int,cchar *,int,int) noex ;
extern int sreqdb_typeset(SREQDB *,int,int,int) noex ;
extern int sreqdb_get(SREQDB *,int,SREQ **) noex ;
extern int sreqdb_findpid(SREQDB *,pid_t,SREQ **) noex ;
extern int sreqdb_thrsdone(SREQDB *,SREQ **) noex ;
extern int sreqdb_del(SREQDB *,int) noex ;
extern int sreqdb_delobj(SREQDB *,SREQ *) noex ;
extern int sreqdb_findfd(SREQDB *,int) noex ;
extern int sreqdb_exiting(SREQDB *,int) noex ;
extern int sreqdb_count(SREQDB *) noex ;
extern int sreqdb_builtrelease(SREQDB *) noex ;
extern int sreqdb_finish(SREQDB *) noex ;

EXTERNC_end


#endif /* SREQDB_INCLUDE */


