#include "CNTKLibrary.h"
#include "CNTKUtils.h"

namespace CNTKUtils {

	using namespace CNTK;

	bool GetVariableByName(std::vector<Variable> variableLists, std::wstring varName, Variable& var)
	{
		for (std::vector<Variable>::iterator it = variableLists.begin(); it != variableLists.end(); ++it)
		{
			if (it->Name().compare(varName) == 0)
			{
				var = *it;
				return true;
			}
		}
		return false;
	}

	bool GetInputVariableByName(FunctionPtr evalFunc, std::wstring varName, Variable& var)
	{
		return GetVariableByName(evalFunc->Arguments(), varName, var);
	}

	bool GetOutputVaraiableByName(FunctionPtr evalFunc, std::wstring varName, Variable& var)
	{
		return GetVariableByName(evalFunc->Outputs(), varName, var);
	}
}