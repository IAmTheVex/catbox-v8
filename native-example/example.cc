// Example module w/ async function

#include <nan.h> // You don't have to use nan, but it's supported
#include <catbox_v8.h>

#include <chrono>
#include <thread>

// `using namespace` is omitted to better show which interfaces exist where.

// `Runnable` is used for async function calls. It is possible that `Run` will never actually be
// called, in the case the isolate is disposed. You should make sure to cleanup in the destructor
// instead of `Run`.
struct TimeoutCallback : public catbox_v8::Runnable {
	// `RemoteHandle` should be used instead of `v8::Persistent` or `nan::Persistent`
	catbox_v8::RemoteHandle<v8::Context> context;
	catbox_v8::RemoteHandle<v8::Function> fn;

	TimeoutCallback(
		catbox_v8::RemoteHandle<v8::Context> context,
		catbox_v8::RemoteHandle<v8::Function> fn
	) :
		context(std::move(context)),
		fn(std::move(fn)) {}

	void Run() override {
		// `v8::Locker` and `v8::HandleScope` are already set up and the isolate is entered. All that is
		// left to do is enter a context.
		v8::Context::Scope context_scope(*context);
		v8::Local<v8::Function> local_fn = *fn;
		v8::Local<v8::Value> argv[0];
		Nan::Call(local_fn, v8::Local<v8::Object>::Cast(Nan::Undefined()), 0, argv);
	}
};

NAN_METHOD(timeout) {
	catbox_v8::IsolateHolder isolate_holder = catbox_v8::IsolateHolder::GetCurrent();
	catbox_v8::RemoteHandle<v8::Context> context(v8::Isolate::GetCurrent()->GetCurrentContext());
	catbox_v8::RemoteHandle<v8::Function> fn(v8::Local<v8::Function>::Cast(info[0]));

	uint32_t ms = Nan::To<uint32_t>(info[1]).FromJust();
	std::thread timeout_thread([=]() mutable {
		// Note that in this closure it is not safe to call into v8! The only thing you can do is
		// schedule a task.
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
		isolate_holder.ScheduleTask(std::make_unique<TimeoutCallback>(std::move(context), std::move(fn)));
	});
	timeout_thread.detach();

	info.GetReturnValue().Set(Nan::Undefined());
}

catbox_v8_MODULE void InitForContext(v8::Isolate* isolate, v8::Local<v8::Context> context, v8::Local<v8::Object> target) {
	Nan::Set(target, Nan::New("timeout").ToLocalChecked(), Nan::GetFunction(Nan::New<v8::FunctionTemplate>(timeout)).ToLocalChecked());
}

NAN_MODULE_INIT(init) {
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	InitForContext(isolate, isolate->GetCurrentContext(), target);
}
NODE_MODULE(native, init);
