/* proglog HEADER */
/* lang=C20 */

/* program-logging */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-09-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Manage program logging operations.

*******************************************************************************/

#ifndef	PROGLOG_INCLUDE
#define	PROGLOG_INCLUDE


#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<stdarg.h>
#include	<userinfo.h>
#include	<localmisc.h>

#include	"defs.h"


EXTERNC_begin

extern int proglog_begin(PROGINFO *,USERINFO *) noex ;
extern int proglog_end(PROGINFO *) noex ;
extern int proglog_intro(PROGINFO *,USERINFO *) noex ;
extern int proglog_checksize(PROGINFO *) noex ;
extern int proglog_check(PROGINFO *) noex ;
extern int proglog_print(PROGINFO *,cchar *,int) noex ;
extern int proglog_printf(PROGINFO *,cchar *,...) noex ;
extern int proglog_vprintf(PROGINFO *,cchar *,va_list) noex ;
extern int proglog_printfold(PROGINFO *,cchar *,cchar *,int) noex ;
extern int proglog_getid(PROGINFO *,char *,int) noex ;
extern int proglog_setid(PROGINFO *,cchar *,int) noex ;
extern int proglog_ssprint(PROGINFO *,cchar *,cchar *,int) noex ;
extern int proglog_ssprintf(PROGINFO *,cchar *,cchar *,...) noex ;
extern int proglog_ssvprintf(PROGINFO *,cchar *,cchar *,va_list) noex ;
extern int proglog_flush(PROGINFO *) noex ;
 
EXTERNC_end


#endif /* PROGLOG_INCLUDE */


