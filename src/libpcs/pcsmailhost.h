/* pcsmailhost HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* PCS get mail-host for a given user */
/* version %I% last-modified %G% */


#ifndef	PCSMAILHOST_INCLUDE
#define	PCSMAILHOST_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<vecstr.h>


EXTERNC_begin

extern int	pcsmailhost(cchar *,char *,int,cchar *) noex ;

EXTERNC_end


#endif /* PCSMAILHOST_INCLUDE */


