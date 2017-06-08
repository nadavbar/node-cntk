#include "CNTKModelObjectWrap.h"

using Nan::EscapableHandleScope;

using v8::Function;
using v8::Handle;
using v8::Local;
using v8::Object;
using v8::Value;

void CNTKModelObjectWrap::Init()
{
	v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
	tpl->SetClassName(Nan::New("CNTKModel").ToLocalChecked());
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	Nan::SetPrototypeMethod(tpl, "eval", Eval);

	constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
}

Handle<Value> CNTKModelObjectWrap::WrapModel(const CNTK::FunctionPtr& model)
{
	EscapableHandleScope scope;

	Local<Value> args[] = { Nan::Undefined() };

	Local<Function> localRef = Nan::New<Function>(constructor());
	//Local<Object> objectInstance = Nan::NewInstance(Nan::GetFunction(localRef).ToLocalChecked(), 0, args).ToLocalChecked
	Local<Object> objectInstance = Nan::NewInstance(localRef, 0, args).ToLocalChecked();
	if (objectInstance.IsEmpty())
	{
		return scope.Escape(Nan::Undefined());
	}


	CNTKModelObjectWrap* objectWrap = Nan::ObjectWrap::Unwrap<CNTKModelObjectWrap>(objectInstance);
	objectWrap->_model = model;

	return scope.Escape(objectInstance);
}

CNTKModelObjectWrap::CNTKModelObjectWrap() {}

CNTKModelObjectWrap::~CNTKModelObjectWrap() {}

NAN_METHOD(CNTKModelObjectWrap::New) 
{
	if (info.IsConstructCall()) 
	{
		CNTKModelObjectWrap *obj = new CNTKModelObjectWrap();
		obj->Wrap(info.This());
		info.GetReturnValue().Set(info.This());
	}
	else 
	{
		const int argc = 1;
		v8::Local<v8::Value> argv[argc] = { info[0] };
		v8::Local<v8::Function> cons = Nan::New(constructor());
		info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
	}
}

NAN_METHOD(CNTKModelObjectWrap::Eval)
{
	// TODO: implement
}