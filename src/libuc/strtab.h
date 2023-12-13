/* strtab INCLUDE */
/* lang=C20 */

/* string table object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRTAB_INCLUDE
#define	STRTAB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<vechand.h>
#include	<hdb.h>
#include	<lookaside.h>
#include	<localmisc.h>


#define	STRTAB			struct strtab_head
#define	STRTAB_CHUNK		struct strtab_chunk

#define	STRTAB_AOBJ		lookaside
#define	STRTAB_MAGIC		0x88776215
#define	STRTAB_MINCHUNKSIZE	40


struct strtab_chunk {
	char		*cdata ;
	int		csz ;		/* allocated extent */
	int		cl ;		/* amount used */
	int		count ;		/* number of items */
} ;

struct strtab_head {
	uint		magic ;
	strtab_chunk	*ccp ;
	vechand		*clp ;		/* chunk-list-pointer */
	hdb		*hlp ;		/* hash-list-pointer */
	lookaside	*lap ;		/* lookaside-list-pointer */
	int		chsize ;
	int		stsize ;	/* "string table" size */
	int		count ;		/* total item count */
} ; /* end struct (strtab) */

typedef struct strtab_head	strtab ;
typedef struct strtab_chunk	strtab_ch ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	strtab_start(strtab *,int) noex ;
extern int	strtab_finish(strtab *) noex ;
extern int	strtab_add(strtab *,cchar *,int) noex ;
extern int	strtab_addfast(strtab *,cchar *,int) noex ;
extern int	strtab_already(strtab *,cchar *,int) noex ;
extern int	strtab_count(strtab *) noex ;
extern int	strtab_strsize(strtab *) noex ;
extern int	strtab_strmk(strtab *,char *,int) noex ;
extern int	strtab_recsize(strtab *) noex ;
extern int	strtab_recmk(strtab *,int *,int) noex ;
extern int	strtab_indlen(strtab *) noex ;
extern int	strtab_indsize(strtab *) noex ;
extern int	strtab_indmk(strtab *,int (*)[3],int,int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* STRTAB_INCLUDE */


