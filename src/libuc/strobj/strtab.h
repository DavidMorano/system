/* strtab HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string table object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRTAB_INCLUDE
#define	STRTAB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vechand.h>		/* LIBU */
#include	<hdb.h>			/* LIBUC */
#include	<lookaside.h>		/* LIBU */


#define	STRTAB			struct strtab_head
#define	STRTAB_CH		struct strtab_chunk
#define	STRTAB_AOBJ		lookaside
#define	STRTAB_MAGIC		0x88776215
#define	STRTAB_MINCHUNKSIZE	40


struct strtab_chunk {
	char		*cdata ;
	int		csz ;		/* allocated extent */
	int		cl ;		/* amount used */
	int		count ;		/* number of items */
} ; /* end struct (strtab_chunk) */

struct strtab_head {
	STRTAB_CH	*chp ;
	vechand		*clp ;		/* chunk-list-pointer */
	hdb		*hlp ;		/* hash-list-pointer */
	lookaside	*lap ;		/* lookaside-list-pointer */
	uint		magval ;
	int		chsz ;
	int		stsz ;		/* "string table" size */
	int		cnt ;		/* total item count */
} ; /* end struct (strtab_head) */

#ifdef	__cplusplus
enum strtabmems {
    	strtabmem_start,
	strtabmem_finish,
	strtabmem_count,
	strtabmem_strsize,
	strtabmem_recsize,
	strtabmem_indlen,
	strtabmem_indsize,
	strtabmem_overlast
} ; /* end enum (strtabmems) */
struct strtab ;
struct strtab_co {
	strtab		*op = nullptr ;
	int		w = -1 ;
	void operator () (strtab *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () (int) noex ;
} ; /* end struct (strtab_co) */
struct strtab : strtab_head {
	strtab_co	start ;
	strtab_co	finish ;
	strtab_co	count ;
	strtab_co	strsize ;
	strtab_co	recsize ;
	strtab_co	indlen ;
	strtab_co	indsize ;
	strtab() noex {
	    start	(this,strtabmem_start) ;
	    finish	(this,strtabmem_finish) ;
	    count	(this,strtabmem_count) ;
	    strsize	(this,strtabmem_strsize) ;
	    recsize	(this,strtabmem_recsize) ;
	    indlen	(this,strtabmem_indlen) ;
	    indsize	(this,strtabmem_indsize) ;
	    clp		= nullptr ;
	} ; /* end ctor */
	strtab(const strtab &) = delete ;
	strtab &operator = (const strtab &) = delete ;
	int add		(cchar *,int) noex ;
	int addfast	(cchar *,int) noex ;
	int already	(cchar *,int) noex ;
	int strmk	(char *,int) noex ;
	int recmk	(int *,int) noex ;
	int indmk	(int (*)[3],int,int) noex ;
	operator int () noex ;
	void dtor() noex ;
	destruct strtab() {
	    if (clp) dtor() ;
	} ;
} ; /* end struct (strtab) */
#else	/* __cplusplus */
typedef STRTAB		strtab ;
#endif /* __cplusplus */

typedef STRTAB_CH	strtab_ch ;

EXTERNC_begin

extern int	strtab_start	(strtab *,int) noex ;
extern int	strtab_finish	(strtab *) noex ;
extern int	strtab_add	(strtab *,cchar *,int) noex ;
extern int	strtab_addfast	(strtab *,cchar *,int) noex ;
extern int	strtab_already	(strtab *,cchar *,int) noex ;
extern int	strtab_count	(strtab *) noex ;
extern int	strtab_strsize	(strtab *) noex ;
extern int	strtab_strmk	(strtab *,char *,int) noex ;
extern int	strtab_recsize	(strtab *) noex ;
extern int	strtab_recmk	(strtab *,int *,int) noex ;
extern int	strtab_indlen	(strtab *) noex ;
extern int	strtab_indsize	(strtab *) noex ;
extern int	strtab_indmk	(strtab *,int (*)[3],int,int) noex ;

EXTERNC_end


#endif /* STRTAB_INCLUDE */


