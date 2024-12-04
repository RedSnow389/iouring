

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

	if (nr_pages > 1) { // if more than one page
		folio = page_folio(pages[0]); // converts from page to folio
		// returns the folio that contains this page
		for (i = 1; i < nr_pages; i++) {
			if (page_folio(pages[i]) != folio) { // different folios -> not physically contiguous 
				folio = NULL; // set folio to NULL as we cannot coalesce into a single entry
				break;
			}
		}
		if (folio) { // if all the pages are in the same folio
			folio_put_refs(folio, nr_pages - 1); 
			nr_pages = 1; // sets nr_pages to 1 as it can be represented as a single folio page
		}
	}
		imu = kvmalloc(struct_size(imu, bvec, nr_pages), GFP_KERNEL); 
	// allocates imu with an array for nr_pages bio_vec(s)
	// bio_vec - a contiguous range of physical memory addresses
	// we need a bio_vec for each (physical) page
    // in the case of a folio - the array of bio_vec(s) will be of size 1
	if (!imu)
		goto done;

	ret = io_buffer_account_pin(ctx, pages, nr_pages, imu, last_hpage);
	if (ret) {
		unpin_user_pages(pages, nr_pages);
		goto done;
	}

	off = (unsigned long) iov->iov_base & ~PAGE_MASK;
	size = iov->iov_len; // sets the size to that passed by the user!
	/* store original address for later verification */
	imu->ubuf = (unsigned long) iov->iov_base; // user-controlled
	imu->ubuf_end = imu->ubuf + iov->iov_len; // calculates the end based on the length
	imu->nr_bvecs = nr_pages; // this would be 1 in the case of folio
	*pimu = imu;
	ret = 0;

	if (folio) { // in case of folio - we need just a single bio_vec (efficiant!)
		bvec_set_page(&imu->bvec[0], pages[0], size, off);
		goto done;
	}
	for (i = 0; i < nr_pages; i++) { 
		size_t vec_len;

		vec_len = min_t(size_t, size, PAGE_SIZE - off);
		bvec_set_page(&imu->bvec[i], pages[i], vec_len, off);
		off = 0;
		size -= vec_len;
	}
done:
	if (ret)
		kvfree(imu);
	kvfree(pages);
	return ret;
}
}
// Here if the iov spans more than a single physical page, the kernel will loop through pages to check if they belong to the same folio. 
