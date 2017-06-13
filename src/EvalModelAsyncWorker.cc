#include "CNTKLibrary.h"
#include "CNTKUtils.h"
#include "CNTKModelObjectWrap.h"
#include "EvalModelAsyncWorker.h"
#include "nan.h"

#include <map>
#include <string>
#include <sstream>

using v8::Array;
using v8::Function;
using v8::Local;
using v8::Object;
using v8::Number;
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
	CNTKEvalInputDataFloat inputData, CNTKEvalOutputVariablesNames outputVariablesNames, const CNTK::DeviceDescriptor &device)
		: AsyncWorker(callback), _model(model), _inputData(inputData), _outputVariablesNames(outputVariablesNames),
		 _device(device), _samplesNum(0) {}

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

			// TODO: support multi variables samples num
			// Allow the use to define a "main" input node
			if (_samplesNum <= 0)
			{
				_samplesNum = it->numberOfSamples;
			}

			CNTK::ValuePtr inputValue = CNTK::MakeSharedObject<CNTK::Value>(CNTK::MakeSharedObject<CNTK::NDArrayView>(inputShape, it->data, true));

			inputVars[inputVar] = inputValue;
		}

		for (auto it = _outputVariablesNames.begin(); it != _outputVariablesNames.end(); it++)
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

			_outputVars[outputVar] = outputValue;
			_outputVarsByName[*it] = outputVar;
		}

		_model->Forward(inputVars, _outputVars, _device);

		// Call the model
		_errorOccured = false;
	}
	catch (const std::invalid_argument& e)
	{
		_errorOccured = true;
		_errorMessage = e.what();
	}
	catch (const std::runtime_error& e)
	{
		_errorOccured = true;
		_errorMessage = e.what();
	}
	catch (const std::exception& e)
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
		result = Nan::New<Object>();

		try
		{
			for (auto it = _outputVarsByName.begin(); it != _outputVarsByName.end(); it++)
			{
				// Get output value
				CNTK::Variable outputVar = it->second;
				CNTK::ValuePtr outputValue = _outputVars[outputVar];

				CNTK::NDShape outputShape = outputVar.Shape().AppendShape({ static_cast<size_t>(_samplesNum) });
				std::vector<float> outputData(outputShape.TotalSize());
				CNTK::NDArrayViewPtr cpuArrayOutput = CNTK::MakeSharedObject<CNTK::NDArrayView>(outputShape, outputData, false);
				cpuArrayOutput->CopyFrom(*outputValue->Data());

				// TODO: Use native array insted? Buffer?
				size_t dataIndex = 0;
				Local<Array> outputArr = Nan::New<Array>();
				auto outputDim = outputVar.Shape()[0];
				for (int i = 0; i < _samplesNum; i++)
				{
					dataIndex = i * outputDim;
					Local<Array> resArr = Nan::New<Array>();
					for (int j = dataIndex, k=0; j < (dataIndex + outputDim); j++, k++)
					{
						Local<Number> val = Nan::New<Number>(outputData[j]);
						Nan::Set(resArr, k, val);
					}
					Nan::Set(outputArr, i, resArr);
				}

				Nan::Set(Nan::To<Object>(result).ToLocalChecked(), Nan::New<String>(reinterpret_cast<const uint16_t*>(it->first.c_str())).ToLocalChecked(), outputArr);
			}
		}
		catch (const std::invalid_argument& e)
		{
			_errorOccured = true;
			_errorMessage = e.what();
		}
		catch (const std::runtime_error& e)
		{
			_errorOccured = true;
			_errorMessage = e.what();
		}
		catch (const std::exception& e)
		{
			_errorOccured = true;
			_errorMessage = e.what();
		}
	}

	if (_errorOccured)
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