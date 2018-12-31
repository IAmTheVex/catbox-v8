#include "allocator.h"
#include "environment.h"
#include <cstdlib>

using namespace v8;

namespace catbox {

bool LimitedAllocator::Check(const size_t length) {
	if (v8_heap + env.extra_allocated_memory + length > next_check) {
		HeapStatistics heap_statistics;
		Isolate* isolate = Isolate::GetCurrent();
		isolate->GetHeapStatistics(&heap_statistics);
		v8_heap = heap_statistics.total_heap_size();
		if (v8_heap + env.extra_allocated_memory + length > limit) {
			// really really dangerous... the tests pass tho'... sooo...
			// if something goes terribly wrong, this might be the cause *wink*
			isolate->LowMemoryNotification();
			isolate->GetHeapStatistics(&heap_statistics);
			v8_heap = heap_statistics.total_heap_size();
			if (v8_heap + env.extra_allocated_memory + length > limit) {
				return false;
			}
		}
		next_check = v8_heap + env.extra_allocated_memory + length + 1024 * 1024;
	}
	return v8_heap + env.extra_allocated_memory + length <= limit;
}

LimitedAllocator::LimitedAllocator(IsolateEnvironment& env, size_t limit) : env(env), limit(limit), v8_heap(1024 * 1024 * 4), next_check(1024 * 1024) {}

void* LimitedAllocator::Allocate(size_t length) {
	if (Check(length)) {
		env.extra_allocated_memory += length;
		return std::calloc(length, 1);
	} else {
		++failures;
		if (length <= 64) { // kMinAddedElementsCapacity * sizeof(uint32_t)
			env.extra_allocated_memory += length;
			env.Terminate();
			return std::calloc(length, 1);
		} else {
			return nullptr;
		}
	}
}

void* LimitedAllocator::AllocateUninitialized(size_t length) {
	if (Check(length)) {
		env.extra_allocated_memory += length;
		return std::malloc(length);
	} else {
		++failures;
		if (length <= 64) {
			env.extra_allocated_memory += length;
			env.Terminate();
			return std::malloc(length);
		} else {
			return nullptr;
		}
	}
}

void LimitedAllocator::Free(void* data, size_t length) {
	env.extra_allocated_memory -= length;
	next_check -= length;
	std::free(data);
}

void LimitedAllocator::AdjustAllocatedSize(ptrdiff_t length) {
	env.extra_allocated_memory += length;
}

int LimitedAllocator::GetFailureCount() const {
	return failures;
}

} // namespace catbox
