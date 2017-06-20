#include "LoadModelAsyncWorker.h"
#include "CNTKModelObjectWrap.h"
#include "nan.h"

using v8::Local;
using v8::Object;
using v8::FunctionTemplate;
using v8::String;
using v8::Integer;


enum CNTKDevices 
{
	GPU = 0,
	CPU = 1
};

// TODO: Move in the future to async method
NAN_METHOD(SetDefaultDeviceSync)
{
	Nan::HandleScope scope;
	if ((info.Length() < 1 || !info[0]->IsInt32()) || (info.Length() > 1 && (!info[0]->IsInt32() || !info[0]->IsInt32())))
	{
		Nan::ThrowTypeError("Bad usage, expected arguments are: device type[integer], optional: GPU device id[integer] (default: 0)");
		return;
	}

	int deviceType = Nan::To<int32_t>(info[0]).FromMaybe(0);

	bool deviceSetResult = false;

	if (deviceType == CNTKDevices::CPU)
	{
		try
		{
			deviceSetResult = CNTK::DeviceDescriptor::TrySetDefaultDevice(CNTK::DeviceDescriptor::CPUDevice());
		}
		catch (...)
		{
			Nan::ThrowError("setDefaultDevice failed, please verify your input parameters");
			return;
		}
	}
	else if (deviceType == CNTKDevices::GPU)
	{
		int gpuDeviceId = 0;

		if (info.Length() > 1)
		{
			gpuDeviceId = Nan::To<int32_t>(info[1]).FromMaybe(0);
		}

		if (gpuDeviceId < 0)
		{
			Nan::ThrowTypeError("Bad usage, GPU device id must be a non negative integer");
			return;
		}

		try
		{
			deviceSetResult = CNTK::DeviceDescriptor::TrySetDefaultDevice(CNTK::DeviceDescriptor::GPUDevice(static_cast<unsigned int>(gpuDeviceId)));
		}
		catch(...)
		{
			Nan::ThrowError("setDefaultDevice failed, please verify your input parameters");
			return;
		}
	}
	else
	{
		Nan::ThrowTypeError("Bad usage, first argument (device type)  value does not much one of the values of the CNTKDevices enum");
		return;
	}

	if (!deviceSetResult)
	{
		Nan::ThrowError("setDefaultDevice failed, please verify your input parameters");
		return;
	}
}

void Init(Local<Object> exports) {
	exports->Set(Nan::New("setDefaultDevice").ToLocalChecked(),
		Nan::New<FunctionTemplate>(SetDefaultDeviceSync)->GetFunction());


	// Init devices enum
	Local<Object> devicesEnum = Nan::New<Object>();
	Nan::Set(devicesEnum, Nan::New<String>("gpu").ToLocalChecked(), Nan::New<Integer>(CNTKDevices::GPU));
	Nan::Set(devicesEnum, Nan::New<String>("cpu").ToLocalChecked(), Nan::New<Integer>(CNTKDevices::CPU));

	Nan::Set(exports, Nan::New<String>("devices").ToLocalChecked(), devicesEnum);
	
	Nan::SetMethod(exports, "setDefaultDeviceSync", SetDefaultDeviceSync);
	Nan::SetMethod(exports, "loadModel", LoadModel);

	CNTKModelObjectWrap::Init();
}

NODE_MODULE(node_win_shortcut_bindings, Init)