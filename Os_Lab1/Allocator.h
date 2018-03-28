struct Header {
	bool isFree;
	size_t nextSize;
	size_t prevSize;
};

void  allocator_init();
void * mem_alloc(size_t);
void * mem_realloc(void *, size_t);
void mem_free(void *);
void allocator_free();
void mem_dump(const char[]);