#include "stdafx.h"

size_t g_size = 2000;
size_t header_size = sizeof(Header);
void *memory;

Header *first;
Header *last;

Header * nextHeader(Header *header);
Header * prevHeader(Header *header);

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
	while (!header->isFree && header != last) { // first free block
		if ((header->nextSize - size) > header_size) // that has more space that we need
			break;
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

	return header + header_size;
}

void * mem_realloc(void *addr, size_t size) {
	if (addr == NULL)
		return mem_alloc(size);

	if (size <= 0)
		return NULL;

	Header *current_header = (Header *)addr - header_size;
	
	int delta_size = current_header->nextSize - size;

	if (delta_size == 0) // no sense to move the block
		return addr;

	if (delta_size > header_size) {
		size_t buf = current_header -> nextSize;

		current_header->nextSize = size; // change block size

		Header *newHeader = nextHeader(current_header); // create new header
		newHeader->isFree = true;						// that is free 
		newHeader->prevSize = size;
		newHeader->nextSize = buf - size - header_size;

		return addr; // address doesn't change if block size is reduced
	} else { // delete this block and call mem_alloc. If it return Null, repair curr block
		Header *prev = prevHeader(current_header);
		Header *next = nextHeader(current_header);
		prev->nextSize += header_size + current_header->nextSize;
		next->prevSize += header_size + current_header->prevSize;

		void * result = malloc(size);
		if (result != NULL) { // we have allocated memory
			return result;
		}
		else { // if result is NULL - there is no free block
			// repair curr block
			prev->nextSize -= header_size + current_header->nextSize; 
			next->prevSize -= header_size + current_header->prevSize;
		}
	}

	return NULL;
}


void mem_free(void *addr) {
	Header *current = (Header *)addr - header_size; // get *Header of current block
	Header *next = nextHeader(current);
	Header *prev = prevHeader(current);

	if (next->isFree && prev->isFree) { // megre 3 blocks
		prev->nextSize += 2 * header_size + current->nextSize + next->nextSize;
		nextHeader(prev)->prevSize = prev->nextSize;
	} else if (next->isFree && !prev->isFree) {
		current->isFree = true;
		current->nextSize += header_size + next->nextSize;
		nextHeader(current)->prevSize = current->nextSize;
	} else if(!next->isFree && prev->isFree) {
		prev->nextSize += header_size + current->nextSize;
		next->prevSize = prev->nextSize;
	} else {
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

void mem_dump() {
	Header* current = (Header *) memory;
	std::cout << "-----Out all Headers-----\n";
	while (current != NULL) {
		std::cout << "isFree=" << (current->isFree ? "Free" : "Busy") << ", prevSize=" << current->prevSize
			<< ", nextSize=" << current->nextSize << "\n";
		current = nextHeader(current);
	}
	std::cout << "\n";
}