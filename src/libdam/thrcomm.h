/* thrcomm INCLUDE */
/* lang=C20 */

/* Thread-Communication (THRCOMM) */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	THRCOMM_INCLUDE
#define	THRCOMM_INCLUDE


#include	<envstandards.h>
#include	<usystem.h>
#include	<ptm.h>
#include	<ptc.h>


#define	THRCOMM		struct thrcomm_head
#define	THRCOMM_MAGIC	0x26293177


struct thrcomm_head {
	uint		magic ;
	PTM		m ;
	PTC		c ;
	volatile int	cmd ;
	volatile int	rrs ;
	volatile int	f_cmd ;
	volatile int	f_exiting ;
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	thrcomm_start(THRCOMM *,int) noex ;
extern int	thrcomm_cmdsend(THRCOMM *,int,int) noex ;
extern int	thrcomm_cmdrecv(THRCOMM *,int) noex ;
extern int	thrcomm_rspsend(THRCOMM *,int,int) noex ;
extern int	thrcomm_rsprecv(THRCOMM *,int) noex ;
extern int	thrcomm_exiting(THRCOMM *) noex ;
extern int	thrcomm_finish(THRCOMM *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* THRCOMM_INCLUDE */


