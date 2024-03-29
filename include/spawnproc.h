/* spawnproc HEADER */
/* lang=C20 */

/* a small object to facilitate spawning processes */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	SPAWNPROC_INCLUDE
#define	SPAWNPROC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>


/* object */
#define	SPAWNPROC_CON		struct spawnproc_control

/* options */
#define	SPAWNPROC_OIGNINTR	(1<<0)		/* ignore interrupts */
#define	SPAWNPROC_OSETSID	(1<<1)		/* set-session-id */
#define	SPAWNPROC_OSETPGRP	(1<<2)		/* set-proces-group ID */
#define	SPAWNPROC_OSETCTTY	(1<<3)		/* set control-term PGID */
#define	SPAWNPROC_OSIGDEFS	(1<<4)		/* set default signals */

/* file descriptor dispositions are */
#define	SPAWNPROC_DINHERIT	0			/* inherit this FD */
#define	SPAWNPROC_DCLOSE	1			/* close this FD */
#define	SPAWNPROC_DCREATE	2			/* create this one */
#define	SPAWNPROC_DOPEN		SPAWNPROC_DCREATE	/* create this one */
#define	SPAWNPROC_DDUP		3			/* DUP this one */
#define	SPAWNPROC_DNULL		4			/* NULL out */

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
} ;

typedef SPAWNPROC_CON	spawnproc_con ;

EXTERNC_begin

extern int spawnproc(spawnproc_con *,cchar *,mainv,mainv) noex ;

EXTERNC_end


#endif /* SPAWNPROC_INCLUDE */


