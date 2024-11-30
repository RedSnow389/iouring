struct sock_common {
	union {
		__addrpair         skc_addrpair;         /*     0     8 */
		struct {
			__be32     skc_daddr;            /*     0     4 */
			__be32     skc_rcv_saddr;        /*     4     4 */
		};                                       /*     0     8 */
	};                                               /*     0     8 */
	union {
		unsigned int       skc_hash;             /*     8     4 */
		__u16              skc_u16hashes[2];     /*     8     4 */
	};                                               /*     8     4 */
	union {
		__portpair         skc_portpair;         /*    12     4 */
		struct {
			__be16     skc_dport;            /*    12     2 */
			__u16      skc_num;              /*    14     2 */
		};                                       /*    12     4 */
	};                                               /*    12     4 */
	short unsigned int         skc_family;           /*    16     2 */
	volatile unsigned char     skc_state;            /*    18     1 */
	unsigned char              skc_reuse:4;          /*    19: 0  1 */
	unsigned char              skc_reuseport:1;      /*    19: 4  1 */
	unsigned char              skc_ipv6only:1;       /*    19: 5  1 */
	unsigned char              skc_net_refcnt:1;     /*    19: 6  1 */

	/* XXX 1 bit hole, try to pack */

	int                        skc_bound_dev_if;     /*    20     4 */
	union {
		struct hlist_node  skc_bind_node;        /*    24    16 */
		struct hlist_node  skc_portaddr_node;    /*    24    16 */
	};                                               /*    24    16 */
	struct proto *             skc_prot;             /*    40     8 */

	...

	/* size: 136, cachelines: 3, members: 25 */
	/* sum members: 135 */
	/* sum bitfield members: 7 bits, bit holes: 1, sum bit holes: 1 bits */
	/* last cacheline: 8 bytes */
};
