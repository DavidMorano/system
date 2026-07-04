/* nameserv HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* name server */
/* version %I% last-modified %G% */


/* revision history:

	- 1996-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	NAMESERV_INCLUDE
#define	NAMESERV_INCLUDE


#include	<envstandards.h>	/* must be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	NAMESERV	struct nameserv_head
#define	NAMESERV_AR	struct nameserv_args
#define	NAMESERV_MAGIC	31815926


struct nameserv_args {
	cchar		*pr ;
	int		timeout ;	/* connection timeout */
	int		options ;
} ; /* end struct */

struct nameserv_head {
	void		*addr ;		/* remote address */
	void		*chp ;		/* connection-handle-pointer */
	uint		magval ;
	int		itype ;
	int		fd ;
	int		addrlen ;
} ; /* end struct */

typedef	NAMESERV	nameserv ;
typedef	NAMESERV_AR	nameserv_args ;

EXTERNC_begin

extern int	nameserv_open(nameserv *,nameserv_args *,cc *,cc *) noex ;
extern int	nameserv_read(nameserv *,char *,int,int) noex ;
extern int	nameserv_write(nameserv *,cchar *,int) noex ;
extern int	nameserv_poll(nameserv *,int) noex ;
extern int	nameserv_close(nameserv *) noex ;
extern int	nameserv_localname(nameserv *,char *,int) noex ;
extern int	nameserv_peername(nameserv *,char *,int) noex ;

EXTERNC_end


#endif /* NAMESERV_INCLUDE */


