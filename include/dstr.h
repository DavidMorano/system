/* dstr HEADER */
/* lang=C20 */

/* string object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DSTR_INCLUDE
#define	DSTR_INCLUDE


#define	DSTR		struct dstr_head


struct dstr_head {
	char		*sbuf ;
	int		slen ;
} ;

typedef DSTR		dstr ;

EXTERNC_begin

extern int dstr_start(DSTR *,cchar *,int) noex ;
extern int dstr_assign(DSTR *,DSTR *) noex ;
extern int dstr_finish(DSTR *) noex ;

EXTERNC_end


#endif /* DSTR_INCLUDE */


