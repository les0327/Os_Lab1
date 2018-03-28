#include "stdafx.h"

size_t g_size = 2000;
size_t header_size = sizeof(Header);
void *memory;

Header *first;
Header *last;

Header * nextHeader(Header *header);
Header * prevHeader(Header *header);
Header * getHeaderFromAddr(void *address);

void  allocator_init() {
	memory = malloc(g_size);
	first = (Header *) memory;

	first->isFree = false; // first free block
	first->prevSize = 0;
	first->nextSize = 0;

	first = nextHeader(first);
	first->isFree = true; // first free block
	first->prevSize = 0;
	first->nextSize = g_size - 3 * header_size;

	last = nextHeader(first); // last busy block
	last->isFree = false;
	last->prevSize = first->nextSize;
	last->nextSize = 0;
}

void * mem_alloc(size_t size) {
	if (size <= 0)
		return NULL;

	Header* header = first;
	// search for first free block
	while (!header->isFree && header != last && (header->nextSize - size) > header_size) { // first free block that has more space that we need
		header = nextHeader(header);
	}

	if (header == last) // no free space
		return NULL;

	size_t buf = header->nextSize;

	header->isFree = false;  // mark block as busy
	header->nextSize = size;

	Header *newHeader = nextHeader(header); // create new header
	newHeader->isFree = true;			    // that is free 
	newHeader->prevSize = size;             
	newHeader->nextSize = buf - size - header_size;

	Header *newNextHeader = nextHeader(newHeader);
	if (newNextHeader->isFree) { // if next block after created is free - merge them
		newHeader->nextSize += header_size + newNextHeader->nextSize;
	}
	else { // else change prevSize
		newNextHeader->prevSize = newHeader->nextSize;
	}

	return (void *)((size_t)header + header_size);
}

void * mem_realloc(void *addr, size_t size) {
	if (addr == NULL)
		return mem_alloc(size);

	if (size <= 0)
		return NULL;

	Header *current_header = getHeaderFromAddr(addr);
	
	int delta_size = current_header->nextSize - size;

	if (delta_size == 0) // no sense to move the block
		return addr;

	if (delta_size > (int) header_size) {
		size_t buf = current_header -> nextSize;

		current_header->nextSize = size; // change block size

		Header *newHeader = nextHeader(current_header); // create new header
		newHeader->isFree = true;						// that is free 
		newHeader->prevSize = size;
		newHeader->nextSize = buf - size - header_size;
		
		nextHeader(newHeader)->prevSize = newHeader->nextSize;

		return addr; // address doesn't change if block size is reduced
	} else { // delete this block and call mem_alloc. If it return Null, repair curr block

		size_t prevSizeBuf = current_header->prevSize;
		size_t nextSizeBuf = current_header->nextSize;

		mem_free(addr);
		//mem_dump("debug - free");
		void * result = mem_alloc(size);
		//mem_dump("debug - malloc");
		if (result != NULL) { // we have allocated memory
			return result;
		}
		else { // if result is NULL - there is no free block
			current_header->isFree = false; // repair curr
			current_header->prevSize = prevSizeBuf;
			current_header->nextSize = nextSizeBuf;

			prevHeader(current_header)->nextSize = current_header->prevSize; // repair prev block
			nextHeader(current_header)->prevSize = current_header->nextSize; // repair next block
		}
	}

	return NULL;
}


void mem_free(void *addr) {
	Header *current = getHeaderFromAddr(addr); // get *Header of current block
	Header *next = nextHeader(current);
	Header *prev = prevHeader(current);

	if (next->isFree && prev->isFree) { // megre 3 blocks
		prev->nextSize += 2 * header_size + current->nextSize + next->nextSize;
		nextHeader(prev)->prevSize = prev->nextSize;
	} else if (next->isFree && !prev->isFree) { // merge next and curr
		current->isFree = true;
		current->nextSize += header_size + next->nextSize;
		nextHeader(current)->prevSize = current->nextSize;
	} else if(!next->isFree && prev->isFree) { // merge prev and curr
		prev->nextSize += header_size + current->nextSize;
		next->prevSize = prev->nextSize;
	} else { // mark curr as free
		current->isFree;
	}
}

void allocator_free() {
	free(memory);
}

Header * nextHeader(Header *header) {
	if (header == last) // only for test
		return NULL;
	return (Header *) ((size_t)header + header->nextSize + header_size);
}

Header * prevHeader(Header *header) {
	return (Header *)((size_t)header - header->prevSize - header_size);;
}

Header * getHeaderFromAddr(void *address) {
	return (Header *)((size_t)address - header_size);
}

void mem_dump(const char message[]) {
	Header* current = (Header *) memory;
	std::cout << "-----Out all Headers-----(" << message << ")\n";
	while (current != NULL) {
		std::cout << "isFree=" << (current->isFree ? "Free" : "Busy") << ", prevSize=" << current->prevSize
			<< ", nextSize=" << current->nextSize << "\n";
		current = nextHeader(current);
	}
	std::cout << "\n";
}