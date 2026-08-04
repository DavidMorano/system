/* rmermsg HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* message for UNLINKD program */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	RMERMSG_INCLUDE
#define	RMERMSG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	RMERMSG		struct rmermsg_head

enum rmermsgtypes {
	rmermsgtype_fname,
	rmermsgtype_unknown,
	rmermsgtype_overlast
} ; /* end enum */

enum rmermsgmems {
    	rmermsgmem_start,
	rmermsgmem_finish,
	rmermsgmem_overlast
} ; /* end enum */

struct rmermsg_head {
	char		*fname ;
	uint		tag ;
	uint		delay ;
	uint		sysid ;
	uid_t		uid ;
	ushort		msglen ;
    	uchar		msgtype ;
} ; /* end struct */

#ifdef	__cplusplus
struct rmermsg ;
struct rmermsg_co {
	rmermsg		*op = nullptr ;
	int		w = -1 ;
	void operator () (rmermsg *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (rmermsg_co) */
struct rmermsg : rmermsg_head {
    	rmermsg_co	start ;
    	rmermsg_co	finish ;
	rmermsg() noex {
	    start	(this,rmermsgmem_start) ;
	    finish	(this,rmermsgmem_finish) ;
	    fname	= nullptr ;
	} ; /* end ctor */
    	int wr		(cchar *,int) noex ;
    	int rd		(char *,int) noex ;
	void dtor() noex ;
	destruct rmermsg() {
	    if (fname) dtor() ;
	} ;
} ; /* end struct (rmermsg) */
#else
typedef	RMERMSG		rmermsg ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	rmermsg_start	(rmermsg *) noex ;
extern int	rmermsg_finish	(rmermsg *) noex ;
extern int	rmermsg_wr	(rmermsg *,cchar *,int) noex ;
extern int	rmermsg_rd	(rmermsg *,char *,int) noex ;
extern int	rmermsg_proc	(rmermsg *,int,char *,int) noex ;

EXTERNC_end


#endif /* RMERMSG_INCLUDE */


