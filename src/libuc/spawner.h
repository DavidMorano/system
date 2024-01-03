/* spawner HEADER */
/* lang=C20 */

/* object to facilitate spanwing processes */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SPAWNER_INCLUDE
#define	SPAWNER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<vecobj.h>
#include	<envhelp.h>
#include	<localmisc.h>


#define	SPAWNER		struct spawner_head

#define	SPAWNER_OIGNINTR	(1<<0)		/* ignore interrupts */
#define	SPAWNER_OSETSID		(1<<1)		/* set-session-id */
#define	SPAWNER_OSETPGRP	(1<<2)		/* set-process-group */
#define	SPAWNER_OSIGDEFS	(1<<3)		/* set default signals */


/* notes on this structure */

/****

This structure is part of the user interface for this facility. The 'disp'
components should be set by the caller to specify the desired disposition for
each of the three FDs of the child program ('0', '1', and '2'). The 'fd'
component serves as an input to the subroutine when the corresponding
disposition is DUP. The 'fd' component is an output from the subroutine when the
corresponding disposition is OPEN.

****/

struct spawner_head {
	const char	**argv ;
	const char	*execfname ;
	vecobj		cmds ;
	ENVHELP		env ;
	pid_t		pid ;
	pid_t		pgrp ;
	int		opts ;
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int spawner_start(SPAWNER *,cchar *,cchar **,cchar **) noex ;
extern int spawner_setsid(SPAWNER *) noex ;
extern int spawner_setpgrp(SPAWNER *,pid_t) noex ;
extern int spawner_setctty(SPAWNER *,int,pid_t) noex ;
extern int spawner_seteuid(SPAWNER *,uid_t) noex ;
extern int spawner_setegid(SPAWNER *,gid_t) noex ;
extern int spawner_sigignores(SPAWNER *) noex ;
extern int spawner_sigignore(SPAWNER *,int) noex ;
extern int spawner_sigdefault(SPAWNER *,int) noex ;
extern int spawner_sighold(SPAWNER *,int) noex ;
extern int spawner_sigrelease(SPAWNER *,int) noex ;
extern int spawner_fdclose(SPAWNER *,int) noex ;
extern int spawner_fdnull(SPAWNER *,int) noex ;
extern int spawner_fddup(SPAWNER *,int) noex ;
extern int spawner_fddup2(SPAWNER *,int,int) noex ;
extern int spawner_fddupto(SPAWNER *,int,int) noex ;
extern int spawner_envset(SPAWNER *,cchar *,cchar *,int) noex ;
extern int spawner_run(SPAWNER *) noex ;
extern int spawner_wait(SPAWNER *,int *,int) noex ;
extern int spawner_finish(SPAWNER *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SPAWNER_INCLUDE */


