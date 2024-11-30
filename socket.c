struct sock {
	struct sock_common         __sk_common;          /*     0   136 */
	/* --- cacheline 2 boundary (128 bytes) was 8 bytes ago --- */
	struct dst_entry *         sk_rx_dst;            /*   136     8 */
	int                        sk_rx_dst_ifindex;    /*   144     4 */
	u32                        sk_rx_dst_cookie;     /*   148     4 */
	socket_lock_t              sk_lock;              /*   152    32 */
	atomic_t                   sk_drops;             /*   184     4 */
	int                        sk_rcvlowat;          /*   188     4 */
	/* --- cacheline 3 boundary (192 bytes) --- */
	struct sk_buff_head        sk_error_queue;       /*   192    24 */
	struct sk_buff_head        sk_receive_queue;     /*   216    24 */
	struct {
		atomic_t           rmem_alloc;           /*   240     4 */
		int                len;                  /*   244     4 */
		struct sk_buff *   head;                 /*   248     8 */
		/* --- cacheline 4 boundary (256 bytes) --- */
		struct sk_buff *   tail;                 /*   256     8 */
	} sk_backlog;                                    /*   240    24 */
	int                        sk_forward_alloc;     /*   264     4 */
	u32                        sk_reserved_mem;      /*   268     4 */
	unsigned int               sk_ll_usec;           /*   272     4 */
	unsigned int               sk_napi_id;           /*   276     4 */
	int                        sk_rcvbuf;            /*   280     4 */

	/* XXX 4 bytes hole, try to pack */

	struct sk_filter *         sk_filter;            /*   288     8 */
	union {
		struct socket_wq * sk_wq;                /*   296     8 */
		struct socket_wq * sk_wq_raw;            /*   296     8 */
	};                                               /*   296     8 */
	struct xfrm_policy *       sk_policy[2];         /*   304    16 */
	/* --- cacheline 5 boundary (320 bytes) --- */
	struct dst_entry *         sk_dst_cache;         /*   320     8 */
	atomic_t                   sk_omem_alloc;        /*   328     4 */
	int                        sk_sndbuf;            /*   332     4 */
	int                        sk_wmem_queued;       /*   336     4 */
	refcount_t                 sk_wmem_alloc;        /*   340     4 */
	long unsigned int          sk_tsq_flags;         /*   344     8 */
	union {
		struct sk_buff *   sk_send_head;         /*   352     8 */
		struct rb_root     tcp_rtx_queue;        /*   352     8 */
	};                                               /*   352     8 */
	struct sk_buff_head        sk_write_queue;       /*   360    24 */
	/* --- cacheline 6 boundary (384 bytes) --- */
	__s32                      sk_peek_off;          /*   384     4 */
	int                        sk_write_pending;     /*   388     4 */
	__u32                      sk_dst_pending_confirm; /*   392     4 */
	u32                        sk_pacing_status;     /*   396     4 */
	long int                   sk_sndtimeo;          /*   400     8 */
	struct timer_list          sk_timer;             /*   408    40 */

	/* XXX last struct has 4 bytes of padding */

	/* --- cacheline 7 boundary (448 bytes) --- */
	__u32                      sk_priority;          /*   448     4 */
	__u32                      sk_mark;              /*   452     4 */
	long unsigned int          sk_pacing_rate;       /*   456     8 */
	long unsigned int          sk_max_pacing_rate;   /*   464     8 */
    // .. many more fields
	/* size: 760, cachelines: 12, members: 92 */
	/* sum members: 754, holes: 1, sum holes: 4 */
	/* sum bitfield members: 16 bits (2 bytes) */
	/* paddings: 2, sum paddings: 6 */
	/* forced alignments: 1 */
	/* last cacheline: 56 bytes */
} __attribute__((__aligned__(8)));
