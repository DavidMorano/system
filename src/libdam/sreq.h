/* sreq HEADER */
/* lang=C++20 */

/* Service-Request */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SREQ_INCLUDE
#define	SREQ_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>		/* for MAXPATHLEN */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<sockaddress.h>
#include	<osetstr.h>
#include	<localmisc.h>		/* |MAXPATHLEN| */

#include	"mfslocinfo.h"
#include	"mfserve.h"
#include	"svcentsub.h"


#define	SREQ		struct sreq
#define	SREQ_FL		struct sreq_flags
#define	SREQ_SNCUR	struct sreq_sncursor
#define	SREQ_MAGIC	0x65918233
#define	SREQ_JOBIDLEN	15		/* same as LOGIDLEN? */
#define	SREQ_SVCBUFLEN	(5*MAXPATHLEN)	/* max service-buffer length */


enum sreqstates {
	sreqstate_acquire,		/* acquire service string */
	sreqstate_svc,			/* have service string */
	sreqstate_program,		/* spawned program (process) */
	sreqstate_thread,		/* spawned thread */
	sreqstate_done,			/* job finished */
	sreqstate_overlast
} ;

struct sreq_sncursor {
	OSETSTR_CUR	cur ;
} ;

struct sreq_flags {
	uint		longopt:1 ;		/* the "long" switch */
	uint		process:1 ;
	uint		thread:1 ;
	uint		shlib:1 ;
	uint		ss:1 ;
	uint		builtout:1 ;
	uint		builtdone:1 ;
	uint		namesvcs:1 ;		/* service names (for 'help') */
} ;

struct sreq_head {
	SOCKADDRESS	sa ;			/* peername socket address */
	SVCENTSUB	ss ;
	OSETSTR		namesvcs ;		/* service names (for 'help') */
	MFSERVE_INFO	binfo ;			/* buuilt-info info */
	void		*sop ;			/* shared-object handle */
	void		*objp ;			/* object pointer */
	cchar		**av ;			/* argument vector */
	cchar		*argstab ;		/* argument strings */
	const char	*peername ;
	const char	*netuser ;
	const char	*netpass ;
	const char	*netident ;
	const char	*svcbuf ;		/* service-buffer */
	const char	*svc ;			/* allocated */
	const char	*subsvc ;
	char		*efname ;		/* Error-File-Name */
	time_t		stime ;			/* time-start */
	time_t		etime ;			/* time-end */
	pid_t		pid ;			/* child PID */
	pthread_t	tid ;			/* child thread ID */
	SREQ_FL		open, f ;
	uint		magic ;
	volatile int	f_exiting ;
	int		ji ;			/* job number in DB */
	int		jsn ;			/* job serial number */
	int		nav ;			/* number arguments */
	int		salen ;			/* peername socket length */
	int		nnames ;		/* number of names */
	int		ifd ;			/* file-descriptor input */
	int		ofd ;			/* output */
	int		efd ;			/* error */
	int		jtype ;			/* job-type */
	int		stype ;			/* sub-type */
	int		state ;			/* job state (see above) */
	int		svclen ;		/* service-length */
	int		trs ;			/* thread return-status */
	char		logid[SREQ_JOBIDLEN+1] ;
} ;

typedef	SREQ		sreq ;
typedef	SREQ_FL		sreq_fl ;
typedef	SREQ_SNCUR	sreq_sncur ;

EXTERNC_begin

extern int sreq_start(SREQ *,cchar *,cchar *,int,int) noex ;
extern int sreq_typeset(SREQ *,int,int) noex ;
extern int sreq_getfd(SREQ *) noex ;
extern int sreq_havefd(SREQ *,int) noex ;
extern int sreq_svcaccum(SREQ *,cchar *,int) noex ;
extern int sreq_svcmunge(SREQ *) noex ;
extern int sreq_svcparse(SREQ *,int) noex ;
extern int sreq_setlong(SREQ *,int) noex ;
extern int sreq_setstate(SREQ *,int) noex ;
extern int sreq_getjsn(SREQ *) noex ;
extern int sreq_getsvc(SREQ *,cchar **) noex ;
extern int sreq_getsubsvc(SREQ *,cchar **) noex ;
extern int sreq_getav(SREQ *,cchar ***) noex ;
extern int sreq_getstate(SREQ *) noex ;
extern int sreq_getstdin(SREQ *) noex ;
extern int sreq_getstdout(SREQ *) noex ;
extern int sreq_ofd(SREQ *) noex ;
extern int sreq_closestdin(SREQ *) noex ;
extern int sreq_closestdout(SREQ *) noex ;
extern int sreq_closefds(SREQ *) noex ;
extern int sreq_svcentbegin(SREQ *,LOCINFO *,SVCENT *) noex ;
extern int sreq_svcentend(SREQ *) noex ;
extern int sreq_exiting(SREQ *) noex ;
extern int sreq_thrdone(SREQ *) noex ;
extern int sreq_sncreate(SREQ *) noex ;
extern int sreq_snadd(SREQ *,cchar *,int) noex ;
extern int sreq_snbegin(SREQ *,SREQ_SNCUR *) noex ;
extern int sreq_snenum(SREQ *,SREQ_SNCUR *,cchar **) noex ;
extern int sreq_snend(SREQ *,SREQ_SNCUR *) noex ;
extern int sreq_sndestroy(SREQ *) noex ;
extern int sreq_builtload(SREQ *,MFSERVE_INFO *) noex ;
extern int sreq_builtrelease(SREQ *) noex ;
extern int sreq_objstart(SREQ *,cchar *,cchar **) noex ;
extern int sreq_objcheck(SREQ *) noex ;
extern int sreq_objabort(SREQ *) noex ;
extern int sreq_objfinish(SREQ *) noex ;
extern int sreq_stderrbegin(SREQ *,cchar *) noex ;
extern int sreq_stderrclose(SREQ *) noex ;
extern int sreq_stderrend(SREQ *) noex ;
extern int sreq_finish(SREQ *) noex ;

EXTERNC_end


#endif /* SREQ_INCLUDE */


