/* mailaddr EADPER */
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


/* mail address */
#ifndef	MAILADDRLEN
#define	MAILADDRLEN		(3 * MAXHOSTNAMELEN)
#endif

#define	MAILADDRTYPE_NOHOST	-1
#define	MAILADDRTYPE_LOCAL	0
#define	MAILADDRTYPE_UUCP	1
#define	MAILADDRTYPE_ARPA	2
#define	MAILADDRTYPE_ARPAROUTE	3

#define	MAILADDR_LOCALHOST	"*LOCAL*"
#define	LOCALHOSTPART		"*LOCAL*"


EXTERNC_begin

extern int addressparse(cchar *,int,char *,char *) noex ;
extern int addressjoin(char *,int,cchar *,cchar *,int) noex ;
extern int addressarpa(char *,int,cchar *,cchar *,int) noex ;

EXTERNC_end


#endif /* MAILADDR_INCLUDE */


