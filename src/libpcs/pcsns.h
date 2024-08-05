/* pcsns HEADER */
/* lang=C20 */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSNS_INCLUDE
#define	PCSNS_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<modload.h>
#include	<localmisc.h>

#include	"pcsnso.h"
#include	"pcsnsreq.h"


#define	PCSNS_MAGIC	0x99447244
#define	PCSNS		struct pcsns_head
#define	PCSNS_CUR	struct pcsns_cursor
#define	PCSNS_CALLS	struct pcsns_callsubs

/* query options */
#define	PCSNS_ONOSERV	PCSNSO_ONOSERV	/* do not call the server */
#define	PCSNS_OPREFIX	PCSNSO_OPREFIX	/* prefix match */


struct pcsns_cursor {
	void	*scp ;		/* SO-cursor pointer */
	uint	magic ;
} ;

struct pcsns_callsubs {
	int	(*open)(void *,cchar *) noex ;
	int	(*setopts)(void *,int) noex ;
	int	(*get)(void *,char *,int,cchar *,int) noex ;
	int	(*curbegin)(void *,void *) noex ;
	int	(*enumerate)(void *,void *,char *,int,int) noex ;
	int	(*curend)(void *,void *) noex ;
	int	(*audit)(void *) noex ;
	int	(*close)(void *) noex ;
} ;

struct pcsns_head {
	modload		loader ;
	PCSNS_CALLS	call ;
	void		*obj ;		/* object pointer */
	uint		magic ;
	int		objsize ;	/* object size */
	int		cursize ;	/* cursor size */
} ;

typedef	PCSNS		pcsns ;
typedef	PCSNS_CUR	pcsns_cur ;
typedef	PCSNS_CALLS	pcsns_calls ;

EXTERNC_begin

extern int pcsns_open(PCSNS *,cchar *) noex ;
extern int pcsns_setopts(PCSNS *,int) noex ;
extern int pcsns_get(PCSNS *,char *,int,cchar *,int) noex ;
extern int pcsns_curbegin(PCSNS *,PCSNS_CUR *) noex ;
extern int pcsns_enum(PCSNS *,PCSNS_CUR *,char *,int,int) noex ;
extern int pcsns_curend(PCSNS *,PCSNS_CUR *) noex ;
extern int pcsns_audit(PCSNS *) noex ;
extern int pcsns_close(PCSNS *) noex ;

EXTERNC_end


#endif /* PCSNS_INCLUDE */


