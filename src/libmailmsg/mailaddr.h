/* mailaddr EADPER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* parse email route addresses into host and local parts */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILADDR_INCLUDE
#define	MAILADDR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<mailaddrtypes.h>	/* LIBMAILMSG */


/* mail-address types */
#define	MAILADDRTYPE_NOHOST	-1
#define	MAILADDRTYPE_LOCAL	mailaddrtype_local
#define	MAILADDRTYPE_UUCP	mailaddrtype_uucp
#define	MAILADDRTYPE_ARPA	mailaddrtype_arpa
#define	MAILADDRTYPE_ARPAROUTE	mailaddrtype_arparoute
/* special token to indicate a local address */
#define	MAILADDR_LOCALHOST	"*LOCAL*"
#define	LOCALHOSTPART		"*LOCAL*"


EXTERNC_begin

extern int mailaddrparse(cchar *,int,char *,char *) noex ;
extern int mailaddrjoin(char *,int,cchar *,cchar *,mailaddrtypes) noex ;
extern int mailaddrarpa(char *,int,cchar *,cchar *,mailaddrtypes) noex ;

EXTERNC_end


#endif /* MAILADDR_INCLUDE */


