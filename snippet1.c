

// An iovec is just a structure that describes a buffer, with the start address of the buffer and its length. Nothing more.

struct iovec
{
	void __user *iov_base;	// the address at which the buffer starts
	__kernel_size_t iov_len; // the length of the buffer in bytes
};
// When we pass a buffer to be registered we pass it as an iovec. Here the *iov pointer in this function points to a structure, 
// containing information about the buffer that the user wants to register.
// An io_mapped_ubuf is a structure that holds the information about a buffer that has been registered to an io_uring instance.

struct io_mapped_ubuf {
	u64		ubuf; // the address at which the buffer starts
	u64		ubuf_end; // the address at which it ends
	unsigned int	nr_bvecs; // how many bio_vec(s) are needed to address the buffer 
	unsigned long	acct_pages;
	struct bio_vec	bvec[]; // array of bio_vec(s)
};

// The last member of io_mapped_buf is an array of bio_vec(s). A bio_vec is kind of like an iovec but for physical memory. It defines a contiguous range of physical memory addresses.

struct bio_vec {
	struct page	*bv_page; // the first page associated with the address range
	unsigned int	bv_len; // length of the range (in bytes)
	unsigned int	bv_offset; // start of the address range relative to the start of bv_page
};

// the pages that the iov spans get pinned to memory ensuring they stay in the main memory and are exempt from paging. 
// An array pages is returned that contains pointers to the struct page(s) that the iov spans and nr_pages gets set to the number of pages.

static int io_sqe_buffer_register(struct io_ring_ctx *ctx, struct iovec *iov,
				  struct io_mapped_ubuf **pimu,
				  struct page **last_hpage)
{
	struct io_mapped_ubuf *imu = NULL;
	struct page **pages = NULL; // important to remember: *struct page* refers to physical pages
	unsigned long off;
	size_t size;
	int ret, nr_pages, i;
	struct folio *folio = NULL;

	*pimu = ctx->dummy_ubuf;
	if (!iov->iov_base) // if base is NULL
		return 0;

	ret = -ENOMEM;
	pages = io_pin_pages((unsigned long) iov->iov_base, iov->iov_len,
				&nr_pages); // pins the pages that the iov occupies
	// returns a pointer to an array of *page* pointers 
	// and sets nr_pages to the number of pinned pages
	if (IS_ERR(pages)) {
		ret = PTR_ERR(pages);
		pages = NULL;
		goto done;
	}
