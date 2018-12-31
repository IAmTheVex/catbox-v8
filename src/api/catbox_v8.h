#pragma once
#ifdef _WIN32
#define CATBOX_V8_MODULE extern "C" __declspec(dllexport)
#else
#define CATBOX_V8_MODULE extern "C"
#endif

#include "../isolate/environment.h"
#include "../isolate/holder.h"
#include "../isolate/remote_handle.h"
#include "../isolate/runnable.h"
#include <memory>

namespace catbox_v8 {
	using Runnable = catbox::Runnable; // pure virtual Run() = 0;

	class IsolateHolder {
		private:
			std::shared_ptr<catbox::IsolateHolder> holder;
			IsolateHolder(std::shared_ptr<catbox::IsolateHolder> holder) : holder(std::move(holder)) {
				catbox::IsolateEnvironment::Scheduler::IncrementUvRef();
			}

		public:
			IsolateHolder(const IsolateHolder& that) : holder(that.holder) {
				catbox::IsolateEnvironment::Scheduler::IncrementUvRef();
			}

			IsolateHolder(IsolateHolder&& that) : holder(std::move(that.holder)) {
				catbox::IsolateEnvironment::Scheduler::IncrementUvRef();
			}

			IsolateHolder& operator=(const IsolateHolder&) = default;
			IsolateHolder& operator=(IsolateHolder&) = default;

			~IsolateHolder() {
				catbox::IsolateEnvironment::Scheduler::DecrementUvRef();
			}

			static IsolateHolder GetCurrent() {
				return IsolateHolder(catbox::IsolateEnvironment::GetCurrentHolder());
			}

			void ScheduleTask(std::unique_ptr<Runnable> runnable) {
				holder->ScheduleTask(std::move(runnable), false, true, false);
			}

			void Release() {
				holder.reset();
			}
	};

	template <typename T>
	class RemoteHandle {
		private:
			std::shared_ptr<catbox::RemoteHandle<T>> handle;

		public:
			explicit RemoteHandle(v8::Local<T> handle) : handle(std::make_shared<catbox::RemoteHandle<T>>(handle)) {}

			auto operator*() const {
				return handle->Deref();
			}

			void Release() {
				handle.reset();
			}
	};
} // namespace catbox_v8
