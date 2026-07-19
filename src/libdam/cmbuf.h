/* cmbuf HEADER */
/* charset=ISO8859-1 */
/* version %I% last-modified %G% */

/* Connection Manager Buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CMBUF_INCLUDE
#define	CMBUF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	CMBUF		struct cmbuf_head
#define	CMBUF_SP	struct cmbuf_space
#define	CMBUF_MAGIC	31815926


struct cmbuf_space {
	char		*bp ;
	int		bl ;
} ; /* end struct */

struct cmbuf_head {
	char		*buf ;
	char		*bp ;
	uint		magval ;
	int		buflen ;
	int		bl ;
} ; /* end struct */

typedef	CMBUF		cmbuf ;
typedef	CMBUF_SP	cmbuf_sp ;

EXTERNC_begin

extern int cmbuf_start		(cmbuf *,cchar *,int) noex ;
extern int cmbuf_added		(cmbuf *,int) noex ;
extern int cmbuf_getspace	(cmbuf *,cmbuf_sp *) noex ;
extern int cmbuf_getln		(cmbuf *,int,cchar **) noex ;
extern int cmbuf_getlastln	(cmbuf *,cchar **) noex ;
extern int cmbuf_finish		(cmbuf *) noex ;

EXTERNC_end


#endif /* CMBUF_INCLUDE */


