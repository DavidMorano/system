/* cm_opener HEADER (Communications-Connection Manager) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Connection Manager */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	cm_opener

	Description:
	This is a communications connection manager object.  This
	object abstracts the details of a particular connection
	from the calling program.

*******************************************************************************/

#ifndef	CMOPENER_INCLUDE
#define	CMOPENER_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
/****/
#include	<expcook.h>		/* LIBUC */
#include	<systems.h>		/* LIBUC */
#include	<sysdialer.h>		/* LIBUC */
/****/
#include	<localmisc.h>		/* LIBU |TIMEBUFLEN| */

#include	"cm_subinfo.h"
#include	"cm.h"


#define	TI_FILECHECK	3

#define	EC		expcook
#define	SY		systems
#define	SY_ENT		systems_ent
#define	ST_CUR		systems_cur
#define	SD		sysdialer
#define	SD_ARGS		sysdialer_args
#define	SD_ENT		sysdialer_ent
#define	SI		cm_subinfo


namespace cm_ns {
    struct opener {
	cm		*op ;
	cm_args		*argp ;
	cchar		*hn ;
	cchar		*sn ;
	mainv		av ;
	SD_ARGS		da{} ;
	expcook		cooks ;
	cm_subinfo	si ;
	int		arglen ;
	opener(cm *ªop,cm_args *ªap,ccp ªhn,ccp ªsn,mainv ªav) noex {
	    op		= ªop ;
	    argp	= ªap ;
	    hn		= ªhn ;
	    sn		= ªsn ;
	    av		= ªav ;
	} ; /* end ctor */
	operator int () noex ;
	int procargs	() noex ;
	int loadcooks	() noex ;
	int procargs	() noex ;
	int tryfind	() noex ;
	int trysave	() noex ;
    } ; /* end struct (opener) */
} /* end namespace (cm_ns) */


#endif /* __cplusplus */
#endif /* CMOPENER_INCLUDE */


