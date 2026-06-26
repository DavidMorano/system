/* jobdb HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* perform various functions on a job */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module is responsible for providing means to store a
	job and the retrieve it later by its PID.

*******************************************************************************/

#ifndef	JOBDB_INCLUDE
#define	JOBDB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecitem.h>		/* LIBUC */


#define	JOBDB		struct jobdb_head
#define	JOBDB_ENT	struct jobdb_ent
#define	JOBDB_JOBIDLEN	15			/* same as LOGIDLEN? */
#define	JOBDB_JOBPREFIX	"jobdb"			/* job-file prefix */
#define	JOBDB_JOBFILETO	(5*3600)		/* job-file time-out */


struct jobdb_ent {
	cchar		*name ;
	char		*ofname ;
	char		*efname ;
	time_t		atime ;			/* job arrival time */
	time_t		stime ;			/* job start time */
	pid_t		pid ;			/* run flag */
	char		jobid[JOBDB_JOBIDLEN + 1] ;
} ; /* end struct */

struct jobdb_head {
	cchar		*tmpdname ;
	vecitem		*dbp ;
	time_t		ti_jobdir ;
} ; /* end struct */

typedef JOBDB		jobdb ;
typedef JOBDB_ENT	jobdb_ent ;

EXTERNC_begin

extern int jobdb_start		(jobdb *,int,cchar *) noex ;
extern int jobdb_newjob		(jobdb *,cchar *,int) noex ;
extern int jobdb_get		(jobdb *,int,jobdb_ent **) noex ;
extern int jobdb_getp		(jobdb *,jobdb_ent *) noex ;
extern int jobdb_findpid	(jobdb *,pid_t,jobdb_ent **) noex ;
extern int jobdb_del		(jobdb *,int) noex ;
extern int jobdb_delent		(jobdb *,jobdb_ent *) noex ;
extern int jobdb_count		(jobdb *) noex ;
extern int jobdb_check		(jobdb *,time_t,int) noex ;
extern int jobdb_finish		(jobdb *) noex ;

EXTERNC_end


#endif /* JOBDB_INCLUDE */


