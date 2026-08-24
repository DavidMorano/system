/* spawner HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* object to facilitate spanwing processes */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	spawner

	Description:
	Here we spawn a process while specifying options for its
	start-up.

	Synopsis:
	int spawner_start(spawner *op,cchar *fname,mainv argv,mainv envv) noex

	Arguments:
	op		pointer to optional file descriptor dispositions
	fname		program to execute
	argv		arguments to program
	envv		environment to program

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#ifndef	SPAWNER_INCLUDE
#define	SPAWNER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecobj.h>		/* LIBUC */
#include	<envhelp.h>		/* LIBUC */


#define	SPAWNER		struct spawner_head
#define	SPAWNER_MAGIC	0x87235939

enum spawneros {
    	spawnero_ignintr,		/* ignore interrupts */
    	spawnero_setsid,		/* set-session-id */
    	spawnero_setpgrp,		/* set-process-group */
    	spawnero_setctty,		/* set control-terminal PFID */
    	spawnero_sigdefs,		/* set default signals */
    	spawnero_overlast
} ; /* end enum (spawneros) */

#ifdef	__cplusplus
struct spawnerms {
    	static constexpr int	ignintr		= (1 << spawnero_ignintr) ;
    	static constexpr int	setsid		= (1 << spawnero_setsid) ;
    	static constexpr int	setpgrp		= (1 << spawnero_setpgrp) ;
    	static constexpr int	setctty		= (1 << spawnero_setctty) ;
    	static constexpr int	sigdefs		= (1 << spawnero_sigdefs) ;
} ; /* end struct (spawnerms) */
#endif /* __cplusplus */

/* options */
#define	SPAWNER_OIGNINTR	(1 << spawnero_ignintr)
#define	SPAWNER_OSETSID		(1 << spawnero_setsid)
#define	SPAWNER_OSETPGRP	(1 << spawnero_setpgrp)
#define	SPAWNER_OSIGDEFS	(1 << spawnero_sigdefs)

/* notes on this structure */

/****

This structure is part of the user interface for this facility.
The 'disp' components should be set by the caller to specify the
desired disposition for each of the three FDs of the child program
('0', '1', and '2').  The 'fd' component serves as an input to the
subroutine when the corresponding disposition is DUP.  The 'fd'
component is an output from the subroutine when the corresponding
disposition is OPEN.

****/

struct spawner_head {
	mainv		argv ;
	cchar		*execfname ;
	vecobj		*cmdp ;
	envhelp		*envp ;
	pid_t		pid ;
	pid_t		pgrp ;
	uint		magval ;
	int		opts ;
} ; /* end struct (spawner_head) */

typedef SPAWNER		spawner ;

EXTERNC_begin

extern int spawner_start	(spawner *,cchar *,mainv,mainv) noex ;
extern int spawner_setsid	(spawner *) noex ;
extern int spawner_setpgrp	(spawner *,pid_t) noex ;
extern int spawner_setctty	(spawner *,int,pid_t) noex ;
extern int spawner_seteuid	(spawner *,uid_t) noex ;
extern int spawner_setegid	(spawner *,gid_t) noex ;
extern int spawner_sigignores	(spawner *) noex ;
extern int spawner_sigignore	(spawner *,int) noex ;
extern int spawner_sigdefault	(spawner *,int) noex ;
extern int spawner_sighold	(spawner *,int) noex ;
extern int spawner_sigrelease	(spawner *,int) noex ;
extern int spawner_fdclose	(spawner *,int) noex ;
extern int spawner_fdnull	(spawner *,int) noex ;
extern int spawner_fddup	(spawner *,int) noex ;
extern int spawner_fddupover	(spawner *,int,int) noex ;
extern int spawner_fddupto	(spawner *,int,int) noex ;
extern int spawner_envset	(spawner *,cchar *,cchar *,int) noex ;
extern int spawner_run		(spawner *) noex ;
extern int spawner_wait		(spawner *,int *,int) noex ;
extern int spawner_finish	(spawner *) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern const spawnerms		spawnerm ;
#endif /* __cplusplus */


#endif /* SPAWNER_INCLUDE */


