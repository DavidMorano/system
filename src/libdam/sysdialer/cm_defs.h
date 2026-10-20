/* cm_defs HEADER (Communications-Connection Manager) */
/* charset=ISO8859-1 */
/* lang=C20 */

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

#ifndef	CMDEFS_INCLUDE
#define	CMDEFS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stddef.h>		/* POSIX® */
#include	<stdlib.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<vecstr.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<expcook.h>		/* LIBUC */
#include	<systems.h>		/* LIBUC */
#include	<sysdialer.h>		/* LIBUC */


#define	VS		vecstr
#define	EC		expcook
#define	SY		systems
#define	SY_ENT		systems_ent
#define	SY_CUR		systems_cur
#define	SD		sysdialer
#define	SD_ARGS		sysdialer_args
#define	SD_ENT		sysdialer_ent
#define	SI		cm_subinfo


#endif /* CMDEFS_INCLUDE */


