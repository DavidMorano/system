/* mailmsghdrval JEADER */
/* lang=C++20 */

/* message header-value handling */
/* version %I% last-modified %G% */


/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

#ifndef	MAILMSGHDRVAL_INCLUDE
#define	MAILMSGHDRVAL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN		MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN		2048
#endif
#endif

/* object defines */
#define	MAILMSGHDRVAL		struct mailmsghdrval_head
#define	MAILMSGHDRVAL_BUFLEN	LINEBUFLEN


struct mailmsghdrval_head {
	char		*vbuf ;	/* initially LINEBUFLEN in size */
	char		vlen ;
	int		vl ;
	int		idx ;
} ;

typedef MAILMSGHDRVAL		mailmsghdrval ;

EXTERNC_begin

extern int mailmsghdrval_start(mailmsghdrval *,int,cchar *,int) noex ;
extern int mailmsghdrval_add(mailmsghdrval *,cchar *,int) noex ;
extern int mailmsghdrval_get(mailmsghdrval *,cchar **,int *) noex ;
extern int mailmsghdrval_clr(mailmsghdrval *) noex ;
extern int mailmsghdrval_finish(mailmsghdrval *) noex ;

EXTERNC_end


#endif /* MAILMSGHDRVAL_INCLUDE */


