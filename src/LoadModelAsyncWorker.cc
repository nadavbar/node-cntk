#include "CNTKLibrary.h"
#include "LoadModelAsyncWorker.h"
#include "nan.h"
#include <string>


using v8::Function;
using v8::Local;
using v8::String;
using v8::Value;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;
using Nan::New;
using Nan::Null;
using Nan::To;
using CNTK::DeviceDescriptor;

using std::wstring;

class LoadModelAsyncWorker : public AsyncWorker {
public:
	LoadModelAsyncWorker(Callback *callback, const wstring &modelFilePath, const DeviceDescriptor &device)
		: AsyncWorker(callback), _modelFilePath(modelFilePath), _device(device) {}
	~LoadModelAsyncWorker() {}

	void Execute() {
		// Load the model
		auto modelFuncPtr = CNTK::Function::Load(_modelFilePath, _device);
	}

	// Executed when the async work is complete
	// this function will be run inside the main event loop
	// so it is safe to use V8 again
	void HandleOKCallback() {
		HandleScope scope;

		Local<Value> argv[] = {
			Null(), 
			Null()
		};

		callback->Call(2, argv);
	}

private:
	wstring _modelFilePath;
	DeviceDescriptor _device;
	//CntkModelWrap _model;
};

NAN_METHOD(LoadModel) {
	
	if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction())
	{
		Nan::ThrowTypeError("Bad usage, expected arguments are: model path[string], completion callback [function]");
		return;
	}

	String::Value modelPathValue(info[0]);
	wstring modelPath = reinterpret_cast<wchar_t*>(*modelPathValue);
	Callback *callback = new Callback(info[1].As<Function>());

	AsyncQueueWorker(new LoadModelAsyncWorker(callback, modelPath, DeviceDescriptor::UseDefaultDevice()));
}