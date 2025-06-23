/* bibleverse HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLEVERSE_INCLUDE
#define	BIBLEVERSE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<modload.h>

#include	<bibleverses.h>


#define	BIBLEVERSE_MAGIC	0x99447246
#define	BIBLEVERSE		struct bibleverse_head
#define	BIBLEVERSE_Q		struct bibleverse_query
#define	BIBLEVERSE_CITE		struct bibleverse_query
#define	BIBLEVERSE_CUR		struct bibleverse_cursor
#define	BIBLEVERSE_INFO		struct bibleverse_information
#define	BIBLEVERSE_CA		struct bibleverse_calls


struct bibleverse_information {
	time_t	dbtime ;		/* db-time */
	time_t	vitime ;		/* vi-time */
	uint	maxbook ;
	uint	maxchapter ;
	uint	nverses ;
	uint	nzverses ;
} ;

struct bibleverse_query {
	uchar	b, c, v ;
} ;

struct bibleverse_cursor {
	void	*scp ;
	uint	magic ;
} ;

EXTERNC_begin
struct bibleverse_calls {
	int	(*open)(void *,cchar *,cchar *) noex ;
	int	(*count)(void *) noex ;
	int	(*read)(void *,char *,int,BIBLEVERSES_QUERY *) noex ;
	int	(*get)(void *,BIBLEVERSES_QUERY *,char *,int) noex ;
	int	(*curbegin)(void *,BIBLEVERSES_CUR *) noex ;
	int	(*enumerate)(void *,BIBLEVERSES_CUR *,
			BIBLEVERSES_QUERY *,char *,int) noex ;
	int	(*curend)(void *,BIBLEVERSES_CUR *) noex ;
	int	(*audit)(void *) noex ;
	int	(*info)(void *,BIBLEVERSES_INFO *) noex ;
	int	(*chapters)(void *,int,uchar *,int) noex ;
	int	(*close)(void *) noex ;
} ;
EXTERNC_end

struct bibleverse_head {
	void		*obj ;		/* object pointer */
	modload		loader ;
	BIBLEVERSE_CA	call ;
	uint		magic ;
	int		objsize ;	/* object size */
	int		cursize ;	/* cursor size */
} ;

typedef	BIBLEVERSE		bibleverse ;
typedef	BIBLEVERSE_Q		bibleverse_q ;
typedef	BIBLEVERSE_CITE		bibleverse_cite ;
typedef	BIBLEVERSE_CUR		bibleverse_cur ;
typedef	BIBLEVERSE_INFO		bibleverse_info ;
typedef	BIBLEVERSE_CA		bibleverse_ca ;

EXTERNC_begin

extern int	bibleverse_open(bibleverse *,cchar *,cchar *) noex ;
extern int	bibleverse_count(bibleverse *) noex ;
extern int	bibleverse_read(bibleverse *,char *,int,bibleverse_q *) noex ;
extern int	bibleverse_get(bibleverse *,bibleverse_q *,char *,int) noex ;
extern int	bibleverse_curbegin(bibleverse *,bibleverse_cur *) noex ;
extern int	bibleverse_enum(bibleverse *,bibleverse_cur *,
			bibleverse_q *,char *,int) noex ;
extern int	bibleverse_curend(bibleverse *,bibleverse_cur *) noex ;
extern int	bibleverse_audit(bibleverse *) noex ;
extern int	bibleverse_info(bibleverse *,bibleverse_info *) noex ;
extern int	bibleverse_chapters(bibleverse *,int,uchar *,int) noex ;
extern int	bibleverse_close(bibleverse *) noex ;

EXTERNC_end


#endif /* BIBLEVERSE_INCLUDE */


