/* dstr INCLUDE */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DSTR_INCLUDE
#define	DSTR_INCLUDE


#define	DSTR		struct dstr_head


struct dstr_head {
	char		*sbuf ;
	int		slen ;
} ;

typedef DSTR		dstr ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int dstr_start(DSTR *,char *,int) noex ;
extern int dstr_assign(DSTR *,DSTR *) noex ;
extern int dstr_finish(DSTR *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* DSTR_INCLUDE */


