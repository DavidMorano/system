/* bitrotate HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* rotate an unsigned integer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BITROTATE_INCLUDE
#define	BITROTATE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdintx.h>		/* extended integer */


EXTERNC_begin

extern uint		bitrotateui(uint,int) noex ;
extern ulong		bitrotateul(ulong,int) noex ;
extern ulonglong	bitrotateull(ulonglong,int) noex ;

EXTERNC_end

#ifdef	__cplusplus

uint		bitrotate(uint v,int n) noex {
    	return bitrotateui(v,n) ;
}
ulong		bitrotate(ulong v,int n) noex {
    	return bitrotateul(v,n) ;
}
ulonglong	bitrotate(ulonglong v,int n) noex {
    	return bitrotateull(v,n) ;
}

#endif /* __cplusplus */


#endif /* BITROTATE_INCLUDE */


