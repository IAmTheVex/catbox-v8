#pragma once
#include <v8.h>

namespace catbox {

class LimitedAllocator : public v8::ArrayBuffer::Allocator {
	private:
		class IsolateEnvironment& env;
		size_t limit;
		size_t v8_heap;
		size_t next_check;
		int failures = 0;


	public:
		bool Check(size_t length);
		explicit LimitedAllocator(class IsolateEnvironment& env, size_t limit);
		void* Allocate(size_t length) final;
		void* AllocateUninitialized(size_t length) final;
		void Free(void* data, size_t length) final;
		// we should no longer count it against the isolate
		void AdjustAllocatedSize(ptrdiff_t length);
		int GetFailureCount() const;
};

} // namespace catbox
