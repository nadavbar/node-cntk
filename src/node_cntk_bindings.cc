#include "nan.h"

using namespace v8;

NAN_METHOD(SetDefaultDevice)
{
	Nan::HandleScope scope;
	// TODO: implement
}

NAN_METHOD(LoadModel)
{
	Nan::EscapableHandleScope scope;
	// TODO: implement
}

void Init(Local<Object> exports) {
	exports->Set(Nan::New("setDefaultDevice").ToLocalChecked(),
		Nan::New<FunctionTemplate>(SetDefaultDevice)->GetFunction());

	exports->Set(Nan::New("loadModel").ToLocalChecked(),
		Nan::New<FunctionTemplate>(LoadModel)->GetFunction());
}

NODE_MODULE(node_win_shortcut_bindings, Init)