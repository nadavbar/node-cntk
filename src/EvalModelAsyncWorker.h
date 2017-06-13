#ifndef EVAL_MODEL_ASYNC_WORKER_H
#define EVAL_MODEL_ASYNC_WORKER_H

#include "CNTKLibrary.h"
#include "nan.h"
#include <map>
#include <vector>
#include <string>

template <class T>
class CNTKEvalInputDataHolder
{
public:
	int numberOfSamples;
	std::wstring inputVaraibleName;
	std::vector<T> data;
};

typedef std::vector<CNTKEvalInputDataHolder<float>> CNTKEvalInputDataFloat;
typedef std::vector<std::wstring> CNTKEvalOutputVariablesNames;

class EvalModelAsyncWorker : public Nan::AsyncWorker {
public:
	EvalModelAsyncWorker(Nan::Callback *callback, CNTK::FunctionPtr model, CNTKEvalInputDataFloat inputData, CNTKEvalOutputVariablesNames outputVariablesNames,const CNTK::DeviceDescriptor &device);
	~EvalModelAsyncWorker();
	void Execute();
	void HandleOKCallback();

private:
	CNTK::FunctionPtr _model;
	CNTK::DeviceDescriptor _device;
	// ... _params
	bool _errorOccured;
	std::string _errorMessage;
	CNTKEvalInputDataFloat _inputData;
	CNTKEvalOutputVariablesNames _outputVariablesNames;
	std::map<std::wstring, CNTK::Variable> _outputVarsByName;
	std::unordered_map<CNTK::Variable, CNTK::ValuePtr> _outputVars;
	int _samplesNum;
};

#endif
