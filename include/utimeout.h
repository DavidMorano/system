/* utimeout HEADER */
/* lang=C20 */

/* manage UNIX� kernal-call (and std-c library) timeouts */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	UTIMEOUT_INCLUDE
#define	UTIMEOUT_INCLUDE


#include	<envstandards.h>
#include	<localmisc.h>


enum utos {
	uto_again,
	uto_intr,
	uto_nomem,
	uto_busy,
	uto_get,
	uto_deadlock,
	uto_io,
	uto_dquot,
	uto_nospc,
	uto_dial,
	uto_open,
	uto_read,
	uto_mfile,
	uto_nfile,
	uto_nosr,
	uto_poll,
	uto_nolck,
	uto_capture,
	uto_overlast
} ; /* end enum (utos) */

extern int	utimeout[] ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int u_timeout(int,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UTIMEOUT_INCLUDE */


