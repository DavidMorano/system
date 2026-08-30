/* thrcomm HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Thread-Communication (THRCOMM) */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	THRCOMM_INCLUDE
#define	THRCOMM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<ptc.h>			/* LIBU */


#define	THRCOMM		struct thrcomm_head
#define	THRCOMM_MAGIC	0x26293177


struct thrcomm_head {
	ptm		*mxp ;
	ptc		*cvp ;
	uint		magval ;
	vol int		cmd ;
	vol int		rrs ;
	vol int		f_cmd ;
	vol int		f_exiting ;
} ; /* end struct */

typedef THRCOMM		thrcomm ;

EXTERNC_begin

extern int	thrcomm_start(thrcomm *,int) noex ;
extern int	thrcomm_cmdsend(thrcomm *,int,int) noex ;
extern int	thrcomm_cmdrecv(thrcomm *,int) noex ;
extern int	thrcomm_rspsend(thrcomm *,int,int) noex ;
extern int	thrcomm_rsprecv(thrcomm *,int) noex ;
extern int	thrcomm_exiting(thrcomm *) noex ;
extern int	thrcomm_finish(thrcomm *) noex ;

EXTERNC_end


#endif /* THRCOMM_INCLUDE */


