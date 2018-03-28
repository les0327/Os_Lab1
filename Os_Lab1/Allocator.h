struct Header {
	bool isFree;
	size_t nextSize;
	size_t prevSize;
};

void  allocator_init();
void * mem_alloc(size_t size);
void * mem_realloc(void *addr, size_t size);
void mem_free(void *addr);
void allocator_free();
void mem_dump();