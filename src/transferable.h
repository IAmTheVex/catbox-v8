#pragma once
#include <v8.h>
#include <memory>

namespace catbox {

class Transferable {
	public:
		Transferable() = default;
		Transferable(const Transferable&) = delete;
		Transferable& operator= (const Transferable&) = delete;
		virtual ~Transferable() = default;
		static std::unique_ptr<Transferable> OptionalTransferOut(const v8::Local<v8::Value>& value);
		static std::unique_ptr<Transferable> TransferOut(const v8::Local<v8::Value>& value);
		virtual v8::Local<v8::Value> TransferIn() = 0;
};

} // namespace catbox
