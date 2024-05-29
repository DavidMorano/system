/* bufstr HEADER */
/* lang=C20 */

/* string buffer object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module can be used to construct strings or messages
	in buffers WITHOUT using the |snsprint(3c)| subroutine.  This
	module is useful when the user does NOT supply a buffer to
	be used as the working store.  Rather, an internally,
	dynamically grown and managed buffer is maintained.  This
	module uses an object, that must be initialized and eventually
	freed, to track the state of the dynamically used internal
	buffer.

*******************************************************************************/

#ifndef	BUFSTR_INCLUDE
#define	BUFSTR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>


#define	BUFSTR		struct bufstr_head
#define	BUFSTR_LEN	100		/* default value */


struct bufstr_head {
	char		*dbuf ;		/* dynamic- buffer */
	int		len ;		/* index (active length) */
	int		dlen ;		/* extent */
	char		sbuf[BUFSTR_LEN + 1] ; /* static-buffer */
} ;

typedef	BUFSTR		bufstr ;

EXTERNC_begin

extern int	bufstr_start(bufstr *) noex ;
extern int	bufstr_chr(bufstr *,int) noex ;
extern int	bufstr_strw(bufstr *,cchar *,int) noex ;
extern int	bufstr_buf(bufstr *,cchar *,int) noex ;
extern int	bufstr_get(bufstr *,cchar **) noex ;
extern int	bufstr_finish(bufstr *) noex ;

EXTERNC_end


#endif /* bufstr_INCLUDE */


