/* sockaddress HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* manipulate socket addresses */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-01-13, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

 	Object:
	sockaddress

	Description:
	See the support file ('sockaddress.cc') for more information
	on what the heck this object is (was) supposed to do.

	Notes:
	I note that the size of this object is not dynamic.  Remember
	that the C-C++ language does not really allow for dynamic
	statically-typed objects (the language wants the size of
	an object to be fixed and static at compile time).  One of
	the goals for this object is to be an alias for the standard
	|sockaddr| object, but with some access methods (unlike
	|sockaddr|).  So in order for this object to contain the
	full path for a UNIX-domain socket, it needs to know what
	the maximum path-length is on any given UNIX system.  I do
	this by using the (semi-standard) 'MAXPATHLEN' "parameter"
	provided by most OSes.  Yes, this is not a dynamic value,
	like it might actually be on many operating systems, but
	remember that some operating systems already severely limit
	the maximum path-length within UNIX-domain sockets.  An
	example operating system that very severely limits the size
	of a UNIX-domain path-length is Apple-Darwin (which limits
	the path-length to something like 104 bytes).

*******************************************************************************/

#ifndef	SOCKADDRESS_INCLUDE
#define	SOCKADDRESS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* |MAXPATHLEN| */
#include	<sys/socket.h>		/* |sockaddr| */
#include	<sys/un.h>		/* |sockaddr_un| */
#include	<netinet/in.h>		/* |sockaddr_in| + |sockaddr_in6| */
#include	<arpa/inet.h>		/* the INET utility subroutines */
#include	<usystem.h>
#include	<localmisc.h>		/* |MAXPATHLEN| */


#define	SOCKADDRESS		struct sockaddress_head
#define	SOCKADDRESS_LEN		szof(union sockaddress_head)
#define	SOCKADDRESS_NAMELEN	szof(union sockaddress_head)
#define	SOCKADDRESS_MAXPATH	MAXPATHLEN


struct sockaddress_head {
    	union {
	    SOCKADDR		a_unspec ;
	    SOCKADDR_UN		a_unix ;
	    SOCKADDR_IN4	a_in4 ;
	    SOCKADDR_IN6	a_in6 ;
	    unsigned char	str[2 + SOCKADDRESS_MAXPATH + 1] ;
	} ;
} ; /* end struct (sockaddress_head) */

#ifdef	__cplusplus
enum sockaddressmems {
	sockaddressmem_getaf,
	sockaddressmem_len,
	sockaddressmem_finish,
	sockaddressmem_overlast
} ;
struct sockaddress ;
struct sockaddress_co {
	sockaddress	*op = nullptr ;
	int		w = -1 ;
	void operator () (sockaddress *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (sockaddress_co) */
struct sockaddress : sockaddress_head {
	sockaddress_co	getaf ;
	sockaddress_co	len ;
	sockaddress_co	finish ;
	sockaddress() noex {
	    getaf	(this,sockaddressmem_getaf) ;
	    len		(this,sockaddressmem_len) ;
	    finish	(this,sockaddressmem_finish) ;
	} ;
	sockaddress(const sockaddress &) = delete ;
	sockaddress &operator = (const sockaddress &) = delete ;
	int start(int,cvoid * = nullptr,int = 0,uint = 0) noex ;
	operator int () noex ;
} ; /* end struct (sockaddress) */
#else	/* __cplusplus */
typedef SOCKADDRESS	sockaddress ;
#endif /* __cplusplus */

EXTERNC_begin

extern int sockaddress_start(sockaddress *,int,cvoid *,int,uint) noex ;
extern int sockaddress_getlen(sockaddress *) noex ;
extern int sockaddress_getaddrlen(sockaddress *) noex ;
extern int sockaddress_gethex(sockaddress *,char *,int) noex ;
extern int sockaddress_getaf(sockaddress *) noex ;
extern int sockaddress_getport(sockaddress *) noex ;
extern int sockaddress_getflow(sockaddress *,uint *) noex ;
extern int sockaddress_getaddr(sockaddress *,void *,int) noex ;
extern int sockaddress_getscope(sockaddress *,uint *) noex ;
extern int sockaddress_getextra(sockaddress *,uint *) noex ;
extern int sockaddress_putaf(sockaddress *,int) noex ;
extern int sockaddress_putport(sockaddress *,int) noex ;
extern int sockaddress_putaddr(sockaddress *,cvoid *) noex ;
extern int sockaddress_finish(sockaddress *) noex ;

EXTERNC_end


#endif /* SOCKADDRESS_INCLUDE */


