/* biblepara SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLEPARA_INCLUDE
#define	BIBLEPARA_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<modload.h>

#include	<bibleparas.h>


#define	BIBLEPARA_MAGIC		0x99447246
#define	BIBLEPARA		struct biblepara_head
#define	BIBLEPARA_CITE		struct biblepara_query
#define	BIBLEPARA_Q		struct biblepara_query
#define	BIBLEPARA_CUR		struct biblepara_cursor
#define	BIBLEPARA_INFO		struct biblepara_information
#define	BIBLEPARA_CA		struct biblepara_calls


struct biblepara_information {
	time_t	dbtime ;		/* db-time */
	time_t	vitime ;		/* vi-time */
	uint	maxbook ;
	uint	maxchapter ;
	uint	nverses ;
	uint	nzverses ;
} ;

struct biblepara_query {
	uchar	b, c, v ;
} ;

struct biblepara_cursor {
	uint	magic ;
	void	*scp ;
} ;

EXTERNC_begin
struct biblepara_calls {
	int	(*open)(void *,cchar *,cchar *) ;
	int	(*count)(void *) ;
	int	(*ispara)(void *,BIBLEPARAS_Q *) ;
	int	(*curbegin)(void *,BIBLEPARAS_CUR *) ;
	int	(*enumerate)(void *,BIBLEPARAS_CUR *,BIBLEPARAS_Q *) ;
	int	(*curend)(void *,BIBLEPARAS_CUR *) ;
	int	(*audit)(void *) ;
	int	(*info)(void *,BIBLEPARAS_INFO *) ;
	int	(*close)(void *) ;
} ;
EXTERNC_end

struct biblepara_head {
	uint		magic ;
	MODLOAD		loader ;
	BIBLEPARA_CALLS	call ;
	void		*obj ;			/* object pointer */
	int		objsize ;		/* object size */
	int		cursize ;		/* cursor size */
} ;

typedef	BIBLEPARA		biblepara ;
typedef	BIBLEPARA_CITE		biblepara_cute ;
typedef	BIBLEPARA_Q		biblepara_q ;
typedef	BIBLEPARA_CUR		biblepara_cur ;
typedef	BIBLEPARA_INFO		biblepara_info ;
typedef	BIBLEPARA_CA		biblepara_ca ;

EXTERNC_begin

extern int	biblepara_open(biblepara *,cchar *,cchar *) noex ;
extern int	biblepara_count(biblepara *) noex ;
extern int	biblepara_ispara(biblepara *,biblepara_q *) noex ;
extern int	biblepara_curbegin(biblepara *,biblepara_cur *) noex ;
extern int	biblepara_enum(biblepara *,biblepara_cur *,biblepara_q *) noex ;
extern int	biblepara_curend(biblepara *,biblepara_cur *) noex ;
extern int	biblepara_audit(biblepara *) noex ;
extern int	biblepara_info(biblepara *,biblepara_info *) noex ;
extern int	biblepara_close(biblepara *) noex ;

EXTERNC_end


#endif /* BIBLEPARA_INCLUDE */


