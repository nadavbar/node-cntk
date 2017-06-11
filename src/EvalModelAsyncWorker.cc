#include "CNTKLibrary.h"
#include "CNTKModelObjectWrap.h"
#include "EvalModelAsyncWorker.h"
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
using std::string;


EvalModelAsyncWorker::EvalModelAsyncWorker(Callback *callback, CNTK::FunctionPtr model, const DeviceDescriptor &device)
		: AsyncWorker(callback), _model(model), _device(device) {}

EvalModelAsyncWorker::~EvalModelAsyncWorker() {}

void EvalModelAsyncWorker::Execute() 
{
	// Load the model
	try
	{
		// Call the model
		_errorOccured = false;
	}
	catch (std::runtime_error e)
	{
		_errorOccured = true;
		_errorMessage = e.what();
	}
}

// Executed when the async work is complete
// this function will be run inside the main event loop
// so it is safe to use V8 again
void EvalModelAsyncWorker::HandleOKCallback() 
{
	HandleScope scope;

	Local<Value> result;
	Local<Value> error;

	// TODO: Convert back the result

	if (!_errorOccured)
	{
		error = Null();
		// TODO: convert the result
		// result = ...
	}
	else
	{
		string errorMessage = "Error occured during call to evalModel: " + _errorMessage;
		error = Nan::Error(Nan::New<String>(errorMessage.c_str()).ToLocalChecked());
		result = Null();
	}

	Local<Value> argv[] = {
		error,
		result
	};

	callback->Call(2, argv);
}