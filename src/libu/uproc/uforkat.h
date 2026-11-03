/* uforkat HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* record and expunge fork hanlers */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UFORKAT_INCLUDE
#define	UFORKAT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin
extern int uforkat_init() noex ;
extern int uforkat_fini() noex ;
extern int uforkat_trackbeg() noex ;
extern int uforkat_trackend() noex ;
EXTERNC_end

EXTERNC_begin
extern int u_forkatrec(void_f,void_f,void_f) noex ;
extern int u_forkatexp(void_f,void_f,void_f) noex ;
EXTERNC_end


#endif /* UFORKAT_INCLUDE */


