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

#ifndef	DS_INCLUDE
#define	DS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdarg.h>		/* |va_list| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<td.h>


/* object define */
#define	DS_MAGIC	0x24182137
#define	DS		struct ds_head
#define	DS_FL		struct ds_flags
/* windows */
#define	DS_WROOT	0
#define	DS_WHEADER	1
#define	DS_WVIEWER	2
/* graphic renditions */
#define	DS_GRBOLD	(1<<0)		/* graphic-rendition bold */
#define	DS_GRUNDER	(1<<1)		/* graphic-rendition underline */
#define	DS_GRBLINK	(1<<2)		/* graphic-rendition blinking */
#define	DS_GRREV	(1<<3)		/* graphic-rendition reverse-video */


struct ds_flags {
	uint		update:1 ;
	uint		mailnew:1 ;	/* new mail arrived */
} ;

struct ds_head {
	cchar		*termtype ;
	TD		*tdp ;		/* terminal display manager */
	DS_FL		fl ;		/* currently unused! */
	uint		magic ;
	int		tfd ;
	int		rows ;
	int		cols ;
} ;

typedef	DS		ds ;
typedef	DS_FL		ds_fl ;

EXTERNC_begin

extern int ds_start(ds *,int,cchar *,int,int) noex ;
extern int ds_subnew(ds *,int,int,int,int) noex ;
extern int ds_subdel(ds *,int) noex ;
extern int ds_getlines(ds *,int) noex ;
extern int ds_setlines(ds *,int,int) noex ;
extern int ds_clear(ds *,int) noex ;
extern int ds_move(ds *,int,int,int) noex ;
extern int ds_ew(ds *,int,int,int) noex ;
extern int ds_el(ds *,int,int) noex ;
extern int ds_ec(ds *,int,int) noex ;
extern int ds_printf(ds *,int,cchar *,...) noex ;
extern int ds_pprintf(ds *,int,int,int,cchar *,...) noex ;
extern int ds_vprintf(ds *,int,cchar *,va_list) noex ;
extern int ds_vpprintf(ds *,int,int,int,cchar *,va_list) noex ;
extern int ds_write(ds *,int,cchar *,int) noex ;
extern int ds_pwrite(ds *,int,int,int,cchar *,int) noex ;
extern int ds_pwritegr(ds *,int,int,int,int,cchar *,int) noex ;
extern int ds_scroll(ds *,int,int) noex ;
extern int ds_flush(ds *) noex ;
extern int ds_suspend(ds *,int,int) noex ;
extern int ds_done(ds *) noex ;
extern int ds_finish(ds *) noex ;

EXTERNC_end


#endif /* DS_INCLUDE */


