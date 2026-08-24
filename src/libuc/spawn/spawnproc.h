/* spawnproc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* a small object to facilitate spawning processes */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	spawnproc

	Description:
	Spawn a process while specifying some options for its start-up.
	This is a cheapy version of this kind of capability as compared
	with other functions of this sort.

	Synopsis:
	int spawnproc(SP *psap,cchar *fname,cchar **argv,cchar **envv) noex

	Arguments:
	psap		pointer to optional file descriptor dispositions
	fname		program to execute
	argv		arguments to program
	envv		environment to program

	Returns:
	>=0		file descriptor to program STDIN and STDOUT
	<0		error (system-return)

*******************************************************************************/

#ifndef	SPAWNPROC_INCLUDE
#define	SPAWNPROC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® |pid_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


/* object */
#define	SPAWNPROC_CON		struct spawnproc_control

enum spawnprocos {
    	spawnproco_ignintr,		/* ignore interrupts */
    	spawnproco_setsid,		/* set-session-id */
    	spawnproco_setpgrp,		/* set-process-group */
    	spawnproco_setctty,		/* set control-terminal PGID */
    	spawnproco_sigdefs,		/* set default signals */
    	spawnproco_overlast
} ; /* end enum (spawnprocos) */

enum spawnprocds {
    	spawnprocd_inherit,		/* inherit this FD */
    	spawnprocd_close,		/* close this FD */
    	spawnprocd_create,		/* create this one */
    	spawnprocd_dup,			/* DUP this one */
    	spawnprocd_null,		/* NULL out */
    	spawnprocd_overlast,
    	spawnprocd_open		= spawnprocd_create /* create this one */
} ; /* end enum (spawnprocds) */

#ifdef	__cplusplus
struct spawnprocms {
    	static constexpr int	ignintr		= (1 << spawnproco_ignintr) ;
    	static constexpr int	setsid		= (1 << spawnproco_setsid) ;
    	static constexpr int	setpgrp		= (1 << spawnproco_setpgrp) ;
    	static constexpr int	setctty		= (1 << spawnproco_setctty) ;
    	static constexpr int	sigdefs		= (1 << spawnproco_sigdefs) ;
} ; /* end struct (spawnprocds) */
#endif /* __cplusplus */

/* options */
#define	SPAWNPROC_OIGNINTR	(1 << spawnproco_ignintr)
#define	SPAWNPROC_OSETSID	(1 << spawnproco_setsid)
#define	SPAWNPROC_OSETPGRP	(1 << spawnproco_setpgrp)
#define	SPAWNPROC_OSETCTTY	(1 << spawnproco_setctty)
#define	SPAWNPROC_OSIGDEFS	(1 << spawnproco_sigdefs)
/* file descriptor dispositions are */
#define	SPAWNPROC_DINHERIT	spawnprocd_inherit
#define	SPAWNPROC_DCLOSE	spawnprocd_close
#define	SPAWNPROC_DCREATE	spawnprocd_create
#define	SPAWNPROC_DOPEN		spawnprocd_create
#define	SPAWNPROC_DDUP		spawnprocd_dup
#define	SPAWNPROC_DNULL		spawnprocd_null

/* notes on this structure */

/****

This structure (below) is part of the user interface for this facility.
The 'disp' components should be set by the caller to specify the desired
disposition for each of the three FDs of the child program ('0', '1', and
'2').  The 'fd' component serves as an input to the subroutine when the
corresponding disposition is DUP.  The 'fd' component is an output from
the subroutine when the corresponding disposition is OPEN.

****/

struct spawnproc_control {
	pid_t		pgrp ;		/* input */
	int		opts ;		/* input */
	int		disp[3] ;	/* input */
	int		fd[3] ;		/* input or output */
	int		fd_ctty ;	/* input */
	int		nice ;		/* input */
} ; /* end struct (spawnproc_control) */

typedef SPAWNPROC_CON	spawnproc_con ;

EXTERNC_begin

extern int spawnproc(spawnproc_con *,cchar *,mainv,mainv) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern const spawnprocms	spawnprocm ;
#endif /* __cplusplus */


#endif /* SPAWNPROC_INCLUDE */


