/* ds HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* low-level terminal-display manager */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-01-20, David A­D­ Morano
	This is a complete rewrite of the trash that performed this
	function previously.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

#ifndef	DISPMGR_INCLUDE
#define	DISPMGR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdarg.h>		/* CSTD |va_list| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<termdisp.h>		/* LIBDAM */


/* object define */
#define	DISPMGR		struct dispmgr_head
#define	DISPMGR_FL	struct dispmgr_flags
#define	DISPMGR_MAGIC	0x24182137
/* windows */
#define	DISPMGR_WROOT	0
#define	DISPMGR_WHEADER	1
#define	DISPMGR_WVIEWER	2
/* graphic renditions */
#define	DISPMGR_GRBOLD	(1<<0)		/* graphic-rendition bold */
#define	DISPMGR_GRUNDER	(1<<1)		/* graphic-rendition underline */
#define	DISPMGR_GRBLINK	(1<<2)		/* graphic-rendition blinking */
#define	DISPMGR_GRREV	(1<<3)		/* graphic-rendition reverse-video */


struct dispmgr_flags {
	uint		update:1 ;
	uint		mailnew:1 ;	/* new mail arrived */
} ; /* end struct */

struct dispmgr_head {
	cchar		*termtype ;
	termdisp	*tdp ;		/* terminal display manager */
	DISPMGR_FL	fl ;		/* currently unused! */
	uint		magval ;
	int		tfd ;
	int		rows ;
	int		cols ;
} ; /* end struct */

typedef	DISPMGR		dispmgr ;
typedef	DISPMGR_FL	dispmgr_fl ;

EXTERNC_begin

extern int dispmgr_start	(dispmgr *,int,cchar *,int,int) noex ;
extern int dispmgr_subnew	(dispmgr *,int,int,int,int) noex ;
extern int dispmgr_subdel	(dispmgr *,int) noex ;
extern int dispmgr_getlines	(dispmgr *,int) noex ;
extern int dispmgr_setlines	(dispmgr *,int,int) noex ;
extern int dispmgr_clear	(dispmgr *,int) noex ;
extern int dispmgr_move		(dispmgr *,int,int,int) noex ;
extern int dispmgr_ew		(dispmgr *,int,int,int) noex ;
extern int dispmgr_el		(dispmgr *,int,int) noex ;
extern int dispmgr_ec		(dispmgr *,int,int) noex ;
extern int dispmgr_printf	(dispmgr *,int,cchar *,...) noex ;
extern int dispmgr_pprintf	(dispmgr *,int,int,int,cchar *,...) noex ;
extern int dispmgr_vprintf	(dispmgr *,int,cchar *,va_list) noex ;
extern int dispmgr_vpprintf	(dispmgr *,int,int,int,cchar *,va_list) noex ;
extern int dispmgr_write	(dispmgr *,int,cchar *,int) noex ;
extern int dispmgr_pwrite	(dispmgr *,int,int,int,cchar *,int) noex ;
extern int dispmgr_pwritegr	(dispmgr *,int,int,int,int,cchar *,int) noex ;
extern int dispmgr_scroll	(dispmgr *,int,int) noex ;
extern int dispmgr_flush	(dispmgr *) noex ;
extern int dispmgr_suspend	(dispmgr *,int,int) noex ;
extern int dispmgr_done		(dispmgr *) noex ;
extern int dispmgr_finish	(dispmgr *) noex ;

EXTERNC_end


#endif /* DISPMGR_INCLUDE */


