/* dial HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* dial out to a TCP port */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DIAL_INCLUDE
#define	DIAL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<dialcprog.h>
#include	<dialcprogmsg.h>
#include	<dialopts.h>
#include	<dialprog.h>
#include	<dialtcp.h>
#include	<dialticotsord.h>
#include	<dialudp.h>
#include	<dialusd.h>
#include	<dialuss.h>
#include	<dialuux.h>


EXTERNC_begin

extern int dialpass(cchar *,int,int) noex ;
extern int dialhttp(cc *,cc *,int,cc *,mainv,int,int) noex ;

EXTERNC_end


#endif /* DIAL_INCLUDE */


