/*
 * Copyright (c) 1982, 1986, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ip_icmp.c	8.2 (Berkeley) 1/4/94
 * ip_icmp.c,v 1.7 1995/05/30 08:09:42 rgrimes Exp
 */

#include "slirp.h"
#include "ip_icmp.h"

struct	icmpstat icmpstat;

/* The message sent when emulating PING */
/* Be nice and tell them it's just a psuedo-ping packet */
char icmp_ping_msg[] = "This is a psuedo-PING packet used by Slirp to emulate ICMP ECHO-REQUEST packets.\n";

extern char icmp_ping_msg[];

/*
 * Process a received ICMP message.
 */
void
icmp_input(m, hlen)
	struct mbuf *m;
	int hlen;
{
	register struct icmp *icp;
	register struct ip *ip = mtod(m, struct ip *);
	int icmplen = ip->ip_len;
	register int i;
	int code;
	struct socket *so;
	struct sockaddr_in addr;
	
	icmpstat.icps_received++;
	
	/*
	 * Locate icmp structure in mbuf, and check
	 * that not corrupted and of at least minimum length.
	 */
	if (icmplen < ICMP_MINLEN) {
		icmpstat.icps_tooshort++;
		goto freeit;
	}
	i = hlen + min(icmplen, ICMP_ADVLENMIN);
	if (m->m_len < i)  {
		icmpstat.icps_tooshort++;
		return;
	}
	ip = mtod(m, struct ip *);
	m->m_len -= hlen;
	m->m_data += hlen;
	icp = mtod(m, struct icmp *);
	if (cksum(m, icmplen)) {
		icmpstat.icps_checksum++;
		goto freeit;
	}
	m->m_len += hlen;
	m->m_data -= hlen;
	
	if (icp->icmp_type > ICMP_MAXTYPE)
		goto freeit;
/*	icmpstat.icps_inhist[icp->icmp_type]++; */
	code = icp->icmp_code;
	switch (icp->icmp_type) {
		
#if 0
#endif
		
	 case ICMP_ECHO:
		icp->icmp_type = ICMP_ECHOREPLY;
		ip->ip_len += hlen;	/* since ip_input deducts this */
		if (ip->ip_dst.s_addr == our_addr.s_addr) {
			icmp_reflect(m);
			return;
		} else {
			if ((so = socreate()) == NULL)
			   goto freeit;
			udp_attach(so);
			so->so_m = m;
			so->so_faddr = ip->ip_dst;
			so->so_fport = htons(7);
			so->so_laddr = ip->ip_src;
			so->so_lport = htons(7);
			so->so_iptos = ip->ip_tos;
			so->so_type = IPPROTO_ICMP;
			so->so_state = SS_ISFCONNECTED;
			
			/* Send the packet */
			addr.sin_family = AF_INET;
			addr.sin_addr = ip->ip_dst;
			addr.sin_port = htons(7);
			sendto(so->s, icmp_ping_msg, strlen(icmp_ping_msg), 0,
			       (struct sockaddr *)&addr, sizeof(addr));
		}
		
		break;
			
	 case ICMP_UNREACH:
		/* XXX? report error? */
	 case ICMP_TIMXCEED:
	 case ICMP_PARAMPROB:
	 case ICMP_SOURCEQUENCH:
	 case ICMP_TSTAMP:
	 case ICMP_MASKREQ:
	 case ICMP_REDIRECT:
		icmpstat.icps_notsupp++;
		m_freem(m);
		break;
		
	 default:
		icmpstat.icps_badtype++;
freeit:
		m_freem(m);
	}

}

/*
 * Reflect the ip packet back to the source
 */
void
icmp_reflect(m)
	struct mbuf *m;
{
	register struct ip *ip = mtod(m, struct ip *);
	int optlen = (ip->ip_hl << 2) - sizeof(struct ip);
	struct in_addr icmp_dst;
	
	icmp_dst = ip->ip_dst;
	ip->ip_dst = ip->ip_src;
	ip->ip_src = icmp_dst;
	ip->ip_ttl = MAXTTL;

	if (optlen > 0) {
		/*
		 * Strip out original options by copying rest of first
		 * mbuf's data back, and adjust the IP length.
		 */
		ip->ip_len -= optlen;
		ip->ip_hl = sizeof(struct ip) >> 2;
		m->m_len -= optlen;
		optlen += sizeof(struct ip);
		memmove((caddr_t)(ip + 1), (caddr_t)ip + optlen,
			 (unsigned)(m->m_len - sizeof(struct ip)));
	}
	icmpstat.icps_reflect++;
	icmp_send(m);
}

/*
 * Send an icmp packet back to the ip level,
 * after supplying a checksum.
 */
void
icmp_send(m)
	register struct mbuf *m;
{
	register struct ip *ip = mtod(m, struct ip *);
	register int hlen;
	register struct icmp *icp;

	hlen = ip->ip_hl << 2;
	m->m_data += hlen;
	m->m_len -= hlen;
	icp = mtod(m, struct icmp *);
	icp->icmp_cksum = 0;
	icp->icmp_cksum = cksum(m, ip->ip_len - hlen);
	m->m_data -= hlen;
	m->m_len += hlen;
	(void) ip_output((struct socket *)NULL, m);
}
