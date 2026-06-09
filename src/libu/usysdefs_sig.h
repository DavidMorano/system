/* usysdefs_sig HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* these are (a multitude) of various UNIX® system pre-processor defines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-09, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This file defines various preprocessor variables for use
	in setting up the UNIX®-system default for various buffer
	sizes.  The real run-time system buffer sizes are (indeed)
	determined at runtime where they may exist.

	Usage:
	Among the most important uses for the data contained herein
	is the BUFSIZE management facility.  It uses the data below
	to set default buffer sizes for those systems that do not
	have dynamicly managed buffer size support in the system
	kernel.

*******************************************************************************/

#ifndef	USYSDEFSSIG_INCLUDE
#define	USYSDEFSSIG_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/utsname.h>		/* POSIX */
#include	<sys/param.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX |S_IS{x}| + S_IF{x}| */
#include	<unistd.h>		/* POSIX |_SC_{x}| + |_PC_{x}| */
#include	<fcntl.h>		/* POSIX |O_{x}| */
#include	<limits.h>		/* CSTD |{xxx}_MIN| + |{xxx}_MAX| */
#include	<signal.h>		/* CSTD |SIG{x}| */


/* missing UNIX® signals */
enum missingsigs {
	missingsig_synthetic = 1000,
	missingsig_pwr,
	missingsig_cancel,
	missingsig_lost,
	missingsig_waiting,
	missingsig_lwp,
	missingsig_freeze,
	missingsig_thaw,
	missingsig_rtmin,
	missingsig_rtmax,
	missingsig_overlast,
} ; /* end enum */

/* signal aliases */
#ifndef	SIGRTMIN
#ifdef	_SIGRTMIN
#define	SIGRTMIN	_SIGRTMIN
#endif
#endif
#ifndef	SIGRTMAX
#ifdef	_SIGRTMAX
#define	SIGRTMAX	_SIGRTMAX
#endif
#endif
/* possibly missing signals */
#ifndef	SIGCLD
#define	SIGCLD		SIGCHLD
#endif
#ifndef	SIGCHILD
#define	SIGCHILD	SIGCHLD
#endif
#ifndef	SIGALARM
#define	SIGALARM	SIGALRM
#endif
#ifndef	SIGPOLL
#define	SIGPOLL		SIGIO
#endif
#ifndef	SIGPWR
#define	SIGPWR		missingsig_pwr
#endif
#ifndef	SIGCANCEL
#define	SIGCANCEL	missingsig_cancel
#endif
#ifndef	SIGLOST
#define	SIGLOST		missingsig_lost
#endif
#ifndef	SIGWAITING
#define	SIGWAITING	missingsig_waiting
#endif
#ifndef	SIGLWP
#define	SIGLWP		missingsig_lwp
#endif
#ifndef	SIGFREEZE
#define	SIGFREEZE	missingsig_freeze
#endif
#ifndef	SIGTHAW
#define	SIGTHAW		missingsig_thaw
#endif
#ifndef	SIGRTMIN
#define	SIGRTMIN	missingsig_rtmin
#endif
#ifndef	SIGRTMAX
#define	SIGRTMAX	missingsig_rtmax
#endif


#endif /* USYSDEFSSIG_INCLUDE */


