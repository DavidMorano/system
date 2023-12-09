/* stdorder INCLUDE */
/* lang=C20 */

/* standard byte-order serializations */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-24, David A­D­ Morano
	This code is newly written but modeled after some code that
	was originally written to get some standardized variable
	serialization for the Ethernet-Controller (ETHCON) development
	work.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	STDORDER_INCLUDE
#define	STDORDER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<stdintx.h>


#ifdef	__cplusplus
extern "C" {
#endif

extern int stdorder_rc(cchar *,char *) noex ;
extern int stdorder_rs(cchar *,short *) noex ;
extern int stdorder_ri(cchar *,int *) noex ;
extern int stdorder_rl(cchar *,long *) noex ;
extern int stdorder_rll(cchar *,longlong *) noex ;

extern int stdorder_ruc(cchar *,uchar *) noex ;
extern int stdorder_rus(cchar *,ushort *) noex ;
extern int stdorder_rui(cchar *,uint *) noex ;
extern int stdorder_rul(cchar *,ulong *) noex ;
extern int stdorder_rull(cchar *,ulonglong *) noex ;

extern int stdorder_wc(char *,int) noex ;
extern int stdorder_ws(char *,int) noex ;
extern int stdorder_wi(char *,int) noex ;
extern int stdorder_wl(char *,long) noex ;
extern int stdorder_wll(char *,longlong) noex ;

extern int stdorder_wuc(char *,uint) noex ;
extern int stdorder_wus(char *,uint) noex ;
extern int stdorder_wui(char *,uint) noex ;
extern int stdorder_wul(char *,ulong) noex ;
extern int stdorder_wull(char *,ulonglong) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* STDORDER_INCLUDE */


