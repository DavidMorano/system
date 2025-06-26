/* pcsconf HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


#ifndef	PCSCONF_INCLUDE
#define	PCSCONF_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>


#define	PCSCONF			struct pcsconf_head

#ifndef	PCSCONF_PCS
#define	PCSCONF_PCS		"/usr/add-on/pcs"
#endif

#define	PCSCONF_FILE1		"etc/conf"
#define	PCSCONF_FILE2		"etc/pcs.conf"

#define	PCSCONF_DEFRELAY	"emsr.lucent.com"
#define	PCSCONF_DEFGATEWAY	"emsr.lucent.com"
#define	PCSCONF_POSTMASTER	"pcs"
#define	PCSCONF_ORGANIZATION	"Lucent Technologies, Inc."
#define	PCSCONF_ORGDOMAIN	"lucent.com"

#define	PCSCONF_USERNAME	"pcs"

#define	PCSCONF_VERSION		"PCS package 3.0"

#define	PCSCONF_LEN		(3 * 1024)

#ifndef	PCS_MSGIDLEN
#define	PCS_MSGIDLEN		(3 * MAXHOSTNAMELEN)
#endif



struct pcsconf_head {
	char	*pcs ;			/* program root (for PCS) */
	char	*nodename ;
	char	*domainname ;
	char	*orgdomain ;		/* company/organization domain */
	char	*pcslogin ;		/* PCS username */
	char	*mailhost ;		/* site MAIL host */
	char	*mailnode ;		/* mailhost's nodename */
	char	*maildomain ;		/* local mail cluster domainname */
	char	*fromhost ;		/* return address host name */
	char	*fromnode ;		/* return address host name */
	char	*uucphost ;		/* site UUCP host (fully qualified) */
	char	*uucpnode ;		/* site UUCP host (a nodename) */
	char	*userhost ;		/* host with user access */
	char	*mailrelay ;		/* company mail relay */
	char	*mailgateway ;		/* company mail gateway */
	char	*postmaster ;		/* sent error mail to here */
	char	*organization ;		/* company/organization string */
	char	*version ;		/* PCS distribution package version */
	uid_t	uid_pcs ;
	gid_t	gid_pcs ;
} ;

typedef	PCSCONF		pcsconf ;

EXTERNC_begin

extern int pcsconf(char *,char *,pcsconf *,vecstr *,vecstr *,
	char *,int) noex ;

EXTERNC_end


#endif /* PCSCONF_INCLUDE */


