/* listenspec HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* this object holds a "listen" specification */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-23, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	LISTENSPEC_INCLUDE
#define	LISTENSPEC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<sockaddress.h>		/* LIBU */
#include	<localmisc.h>		/* |MAXNAMELEN| + |MAXPATHLEN| */


#define	LISTENSPEC		struct listenspec_head
#define	LISTENSPEC_FL		struct listenspec_flags
#define	LISTENSPEC_INFO		struct listenspec_information
#define	LISTENSPEC_PASS		struct listenspec_conpass
#define	LISTENSPEC_USS		struct listenspec_conuss
#define	LISTENSPEC_TCP		struct listenspec_contcp
#define	LISTENSPEC_CONN		struct listenspec_connect
#define	LISTENSPEC_MAGIC	0x57245332
#define	LISTENSPEC_TYPELEN	MAXNAMELEN
#define	LISTENSPEC_ADDRLEN	(MAXPATHLEN + 20)

enum lstenspecos {
    listenspeco_reuse,
    listenspeco_active,
    listenspeco_delpend,
    listenspeco_broken,
    listenspeco_overlast
} ; /* end enum (listenspecos) */

/* options */
#define	LISTENSPEC_MREUSE	(1<<0)	/* reuse address */
/* states */
#define	LISTENSPEC_MACTIVE	(1<<0)	/* is active */
#define	LISTENSPEC_MDELPEND	(1<<1)	/* delete is pending */
#define	LISTENSPEC_MBROKEN	(1<<2)	/* activate failed (broken) */

#ifdef	__cplusplus
struct listenspecms {
    static constexpr int	reuse	=	(1 << listenspeco_reuse) ;
    static constexpr int	active	=	(1 << listenspeco_active) ;
    static constexpr int	delpend	=	(1 << listenspeco_delpend) ;
    static constexpr int	broken	=	(1 << listenspeco_broken) ;
} ; /* end struct (listenspecms) */
#endif /* __cplusplus */

struct listenspec_information {
	int		state ;
	char		type[LISTENSPEC_TYPELEN + 1] ;
	char		addr[LISTENSPEC_ADDRLEN + 1] ;
} ; /* end struct */

struct listenspec_conpass {
	cchar		*fname ;	/* dynamic allocation */
	int		mode ;
} ; /* end struct */

struct listenspec_conuss {
	cchar		*fname ;	/* dynamic allocation */
	int		mode ;
	sockaddress	sa ;
} ; /* end struct */

struct listenspec_contcp {
	void		*a ;		/* dynamic buffer allocation */
	cchar		*af ;
	cchar		*host ;
	cchar		*port ;
	sockaddress	sa ;
} ; /* end struct */

struct listenspec_connect {
	cchar		*fname ;	/* dynamic allocation */
	int		mode ;
} ; /* end struct */

struct listenspec_flags {
	uint		active:1 ;	/* actively listening */
	uint		delmark:1 ;	/* marked for deletion */
	uint		broken:1 ;	/* activate attempt failed */
	uint		reuse:1 ;	/* re-use address */
} ; /* end struct */

struct listenspec_head {
	void		*info ;		/* particular listener information */
	cchar		*prlocal ;
	LISTENSPEC_FL	fl ;
	uint		magval ;
	int		ltype ;		/* "listen" type */
	int		fd ;
	int		rs_error ;
} ; /* end struct */

typedef	LISTENSPEC		listenspec ;
typedef	LISTENSPEC_FL		listenspec_fl ;
typedef	LISTENSPEC_INFO		listenspec_info ;
typedef	LISTENSPEC_PASS		listenspec_pass ;
typedef	LISTENSPEC_USS		listenspec_uss ;
typedef	LISTENSPEC_TCP		listenspec_tcp ;
typedef	LISTENSPEC_CONN		listenspec_conn ;

EXTERNC_begin

extern int listenspec_start	(listenspec *,int,mainv) noex ;
extern int listenspec_issame	(listenspec *,listenspec *) noex ;
extern int listenspec_active	(listenspec *,int,int) noex ;
extern int listenspec_isactive	(listenspec *) noex ;
extern int listenspec_delset	(listenspec *,int) noex ;
extern int listenspec_delmarked	(listenspec *) noex ;
extern int listenspec_getfd	(listenspec *) noex ;
extern int listenspec_gettype	(listenspec *) noex ;
extern int listenspec_accept	(listenspec *,void *,int *,int) noex ;
extern int listenspec_getinfo	(listenspec *,listenspec_info *) noex ;
extern int listenspec_clear	(listenspec *) noex ;
extern int listenspec_geterr	(listenspec *,int *) noex ;
extern int listenspec_finish	(listenspec *) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern const listenspecms	listenspecm ;
#endif /* __cplusplus */


#endif /* LISTENSPEC_INCLUDE */


