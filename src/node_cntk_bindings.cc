#include "LoadModelAsyncWorker.h"
#include "CNTKModelObjectWrap.h"
#include "nan.h"

using v8::Local;
using v8::Object;
using v8::FunctionTemplate;

NAN_METHOD(SetDefaultDevice)
{
	Nan::HandleScope scope;
	// TODO: implement
}

void Init(Local<Object> exports) {
	exports->Set(Nan::New("setDefaultDevice").ToLocalChecked(),
		Nan::New<FunctionTemplate>(SetDefaultDevice)->GetFunction());

	exports->Set(Nan::New("loadModel").ToLocalChecked(),
		Nan::New<FunctionTemplate>(LoadModel)->GetFunction());

	CNTKModelObjectWrap::Init();
}

NODE_MODULE(node_win_shortcut_bindings, Init)