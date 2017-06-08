#ifndef CNTK_MODEL_OBJECT_WRAP_H
#define CNTK_MODEL_OBJECT_WRAP

#include "CNTKLibrary.h"
#include "Nan.h"
#include <string>

class CNTKModelObjectWrap : public Nan::ObjectWrap {
public:
	static void Init();
	static v8::Handle<v8::Value> WrapModel(const CNTK::FunctionPtr& model);

private:
	explicit CNTKModelObjectWrap();
	~CNTKModelObjectWrap();

	static NAN_METHOD(New);
	static NAN_METHOD(Eval);

	static inline Nan::Persistent<v8::Function> & constructor() {
		static Nan::Persistent<v8::Function> my_constructor;
		return my_constructor;
	}

	CNTK::FunctionPtr _model;
};

#endif
