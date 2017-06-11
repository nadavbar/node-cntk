#ifndef LOAD_MODEL_ASYNC_WORKER_H
#define LOAD_MODEL_ASYNC_WORKER_H

#include "CNTKLibrary.h"
#include "nan.h"
#include <string>

class EvalModelAsyncWorker : public Nan::AsyncWorker {
public:
	EvalModelAsyncWorker(Nan::Callback *callback, CNTK::FunctionPtr model, const CNTK::DeviceDescriptor &device);
	~EvalModelAsyncWorker();
	void Execute();
	void HandleOKCallback();

private:
	CNTK::FunctionPtr _model;
	CNTK::DeviceDescriptor _device;
	// ... _params
	bool _errorOccured;
	std::string _errorMessage;
};

#endif
