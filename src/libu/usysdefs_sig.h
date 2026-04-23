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
#include	<sys/types.h>
#include	<sys/utsname.h>
#include	<sys/param.h>
#include	<sys/stat.h>		/* |S_IS{x}| + S_IF{x}| */
#include	<limits.h>		/* |{xxx}_MIN| + |{xxx}_MAX| */
#include	<signal.h>		/* |SIG{x}| */
#include	<unistd.h>		/* |_SC_{x}| + |_PC_{x}| */
#include	<fcntl.h>		/* |O_{x}| */


/* missing UNIX® signals */
enum missingsignals {
	missingsignal_pwr = 1000,
	missingsignal_cancel,
	missingsignal_lost,
	missingsignal_waiting,
	missingsignal_lwp,
	missingsignal_freeze,
	missingsignal_thaw,
	missingsignal_rtmin,
	missingsignal_rtmax,
	missingsignal_overlast,
	missingsignal_start = missingsignal_pwr
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
#define	SIGPWR		missingsignal_pwr
#endif
#ifndef	SIGCANCEL
#define	SIGCANCEL	missingsignal_cancel
#endif
#ifndef	SIGLOST
#define	SIGLOST		missingsignal_lost
#endif
#ifndef	SIGWAITING
#define	SIGWAITING	missingsignal_waiting
#endif
#ifndef	SIGLWP
#define	SIGLWP		missingsignal_lwp
#endif
#ifndef	SIGFREEZE
#define	SIGFREEZE	missingsignal_freeze
#endif
#ifndef	SIGTHAW
#define	SIGTHAW		missingsignal_thaw
#endif
#ifndef	SIGRTMIN
#define	SIGRTMIN	missingsignal_rtmin
#endif
#ifndef	SIGRTMAX
#define	SIGRTMAX	missingsignal_rtmax
#endif


#endif /* USYSDEFSSIG_INCLUDE */


