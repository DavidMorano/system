/* ass HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* allocated string */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ASS_INCLUDE
#define	ASS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	ASS		struct ass_head


struct ass_head {
	char		*rbuf ;
	int		rlen ;
	int		ext ;
} ; /* end struct (ass_head) */

#ifdef	__cplusplus
enum assmems {
	assmem_start,
	assmem_len,
	assmem_finish,
	assmem_overlast
} ;
struct ass ;
struct ass_co {
	ass		*op = nullptr ;
	int		w = -1 ;
	void operator () (ass *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (ass_co) */
struct ass : ass_head {
	ass_co		start ;
	ass_co		len ;
	ass_co		finish ;
	ass() noex {
	    start	(this,assmem_start) ;
	    len		(this,assmem_len) ;
	    finish	(this,assmem_finish) ;
	    rbuf = nullptr ;
	} ;
	ass(const ass &) = delete ;
	ass &operator = (const ass &) = delete ;
	int addchr(int) noex ;
	void dtor() noex ;
	destruct ass() {
	    if (rbuf) dtor() ;
	} ;
} ; /* end struct (ass) */
#else	/* __cplusplus */
typedef ASS		ass ;
#endif /* __cplusplus */

EXTERNC_begin

extern int ass_start(ass *) noex ;
extern int ass_addchr(ass *,int) noex ;
extern int ass_finish(ass *) noex ;

EXTERNC_end


#endif /* ASS_INCLUDE */


