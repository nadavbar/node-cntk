#ifndef CNTK_UTILS
#define CNTK_UTILS

#include "CNTKLibrary.h"
#include <string>
#include <vector>

namespace CNTKUtils {

	bool GetVariableByName(std::vector<CNTK::Variable> variableLists, std::wstring varName, CNTK::Variable& var);

	bool GetInputVariableByName(CNTK::FunctionPtr evalFunc, std::wstring varName, CNTK::Variable& var);

	bool GetOutputVaraiableByName(CNTK::FunctionPtr evalFunc, std::wstring varName, CNTK::Variable& var);

}

#endif