#include "CNTKLibrary.h"
#include "CNTKUtils.h"
#include "CNTKModelObjectWrap.h"
#include "EvalModelAsyncWorker.h"
#include "nan.h"

#include <map>
#include <string>
#include <sstream>


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
using std::map;
using std::stringstream;
using std::unordered_map;


EvalModelAsyncWorker::EvalModelAsyncWorker(Nan::Callback *callback, CNTK::FunctionPtr model, 
	CNTKEvalInputDataFloat inputData, CNTKEvalOutputNodesNames outputNodesNames, const CNTK::DeviceDescriptor &device)
		: AsyncWorker(callback), _model(model), _inputData(inputData), _outputNodesNames(outputNodesNames), _device(device) {}

EvalModelAsyncWorker::~EvalModelAsyncWorker() {}

void EvalModelAsyncWorker::Execute() 
{
	// Load the model
	try
	{
		unordered_map<CNTK::Variable, CNTK::ValuePtr> inputVars;
		for (auto it = _inputData.begin(); it != _inputData.end(); it++)
		{
			CNTK::Variable inputVar;
			// TODO: optimize this such that the model wrap will hold this instead
			if (!CNTKUtils::GetInputVariableByName(_model, it->inputVaraibleName, inputVar))
			{
				stringstream errorMessageStream;
				errorMessageStream << "Input variable: '" << it->inputVaraibleName.c_str() << "' was not found in model.";
				_errorMessage = errorMessageStream.str();
				_errorOccured = true;
				return;
			}

			CNTK::NDShape inputShape = inputVar.Shape().AppendShape({ 1, static_cast<size_t>(it->numberOfSamples) });
			CNTK::ValuePtr inputValue = CNTK::MakeSharedObject<CNTK::Value>(CNTK::MakeSharedObject<CNTK::NDArrayView>(inputShape, it->data, true));

			inputVars[inputVar] = inputValue;
		}

		unordered_map<CNTK::Variable, CNTK::ValuePtr> outputVars;
		for (auto it = _outputNodesNames.begin(); it != _outputNodesNames.end(); it++)
		{
			CNTK::Variable outputVar;
			if (!CNTKUtils::GetOutputVaraiableByName(_model, *it, outputVar))
			{
				stringstream errorMessageStream;
				errorMessageStream << "Output variable: '" << it->c_str() << "' was not found in model.";
				_errorMessage = errorMessageStream.str();
				_errorOccured = true;
				return;
			}

			CNTK::ValuePtr outputValue;

			outputVars[outputVar] = outputValue;
			_outputVarsByName[*it] = outputValue;
		}

		_model->Forward(inputVars, outputVars, _device);
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