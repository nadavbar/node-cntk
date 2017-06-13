#include "CNTKModelObjectWrap.h"
#include "EvalModelAsyncWorker.h"

#include <string>
#include <vector>

using Nan::EscapableHandleScope;

using v8::Array;
using v8::Function;
using v8::Handle;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;
using Nan::Callback;

using std::wstring;
using std::vector;

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

void CNTKModelObjectWrap::JsInputToCntk(Handle<Object> inputsObj, Handle<Array> outputsArr, CNTKEvalInputDataFloat& inputDataOut, CNTKEvalOutputVariablesNames& outputVariablesNamesOut)
{
	Nan::HandleScope scope;
	
	// Get the name of the output nodes
	for (unsigned int i=0; i < outputsArr->Length(); i++)
	{
		Local<String> outputNode = Nan::To<String>(Nan::Get(outputsArr, i).ToLocalChecked()).ToLocalChecked();
		String::Value outputNodeVal(outputNode);
		wstring outputNodeName(reinterpret_cast<wchar_t*>(*outputNodeVal));
		outputVariablesNamesOut.push_back(outputNodeName);
	}

	// get the input value names & values
	Local<Array> inputKeyNames = Nan::GetPropertyNames(inputsObj).ToLocalChecked();
	for (unsigned int i=0; i < inputKeyNames->Length(); i++)
	{
		CNTKEvalInputDataHolder<float> inputData;
		Local<String> inputNode = Nan::To<String>(Nan::Get(inputKeyNames, i).ToLocalChecked()).ToLocalChecked();
		String::Value inputNodeVal(inputNode);
		inputData.inputVaraibleName = reinterpret_cast<wchar_t*>(*inputNodeVal);
		
		Local<Object> dataObj = Nan::To<Object>(Nan::Get(inputsObj, inputNode).ToLocalChecked()).ToLocalChecked();
		
		// get number of rows:
		Local<String> lengthSymb = Nan::New<String>("length").ToLocalChecked();
		inputData.numberOfSamples = Nan::To<int32_t>(Nan::Get(dataObj, lengthSymb).ToLocalChecked()).FromMaybe(0);
		
		// Insert object data to each row
		// TODO: We might be able to optimize the initialiation by resizing according to the input data shape
		// for now just leave this as is and let std do the resizing for us
		for (int j=0; j < inputData.numberOfSamples; j++)
		{
			Local<Object> entryObj = Nan::To<Object>(Nan::Get(dataObj, j).ToLocalChecked()).ToLocalChecked();
			// TODO: We might be able to optimize this for networks with fixes length input by calling this per items
			int itemsCount = Nan::To<int32_t>(Nan::Get(entryObj, lengthSymb).ToLocalChecked()).FromMaybe(0);
			for (int k=0; k < itemsCount; k++)
			{
				float value = static_cast<float>(Nan::To<double_t>(Nan::Get(entryObj, k).ToLocalChecked()).FromMaybe(0.0));
				inputData.data.push_back(value);
			}
		}

		// TODO: we might need to return the number of samples as well
		inputDataOut.push_back(inputData);
	}
	
}

NAN_METHOD(CNTKModelObjectWrap::Eval) {
	Nan::HandleScope scope;
	if (info.Length() < 3 || !info[0]->IsObject() || !info[1]->IsArray() || !info[2]->IsFunction())
	{
		Nan::ThrowTypeError("Bad usage, expected arguments are: input args[key: input node name (string), value: input data (array of arrays)], output node names[array of strings], completion callback [function]");
		return;
	}

	Local<Object> inputDataObj = Nan::To<Object>(info[0]).ToLocalChecked();
	Local<Array> outputNodesArr = info[1].As<Array>();

	CNTKEvalInputDataFloat inputData;
	CNTKEvalOutputVariablesNames outputVariables;
	JsInputToCntk(inputDataObj, outputNodesArr, inputData, outputVariables);

	CNTKModelObjectWrap* objectWrap = Nan::ObjectWrap::Unwrap<CNTKModelObjectWrap>(info.This());
	Callback *callback = new Callback(info[2].As<Function>());

	AsyncQueueWorker(new EvalModelAsyncWorker(callback, objectWrap->_model, inputData, outputVariables, CNTK::DeviceDescriptor::UseDefaultDevice()));
}