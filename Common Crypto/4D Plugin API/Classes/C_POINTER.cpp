/*
 *  C_POINTER.cpp
 *  4D Plugin
 *
 *  Created by miyako on 11/01/03.
 *
 */

#include "C_POINTER.h"

void C_POINTER::fromParamAtIndex(PackagePtr pParams, uint16_t index)
{	
	if(index)
	{
		if (PA_IsCompiled(1))
		{
			PA_Variable *ptr = ((PA_Variable*)pParams[index - 1]);
			
			if (ptr->fType == eVK_Pointer)
				if (!(ptr->uValue.fPointer == 0))
					this->_ptr = *ptr->uValue.fPointer;
		}else{
			this->_ptr = ((PA_Pointer)pParams[index - 1]);
		}
		
		_ptrType = PA_GetPointerKind(this->_ptr);
		_ptrValueType = PA_GetPointerValueKind(this->_ptr);
	}
}

void C_POINTER::getPointerBlock(PointerBlock *pointerBlock)
{
	switch (this->_ptrType)
	{
		case ePK_PointerToVariable:	
			
			pointerBlock->fClass = this->_ptr->fClass;
			pointerBlock->fScope = this->_ptr->fScope;
			pointerBlock->uValue.fVariable.fTag = this->_ptr->uValue.fVariable.fTag;
			pointerBlock->uValue.fVariable.fIndice = this->_ptr->uValue.fVariable.fIndice;			
			
			memcpy(pointerBlock->uValue.fVariable.fName,
				   this->_ptr->uValue.fVariable.fName,
				   sizeof(this->_ptr->uValue.fVariable.fName));
			
			break;
			
		case ePK_PointerToField:	
			
			pointerBlock->fClass = this->_ptr->fClass;
			pointerBlock->fScope = this->_ptr->fScope;
			pointerBlock->uValue.fTableField.fTable = this->_ptr->uValue.fTableField.fTable;
			pointerBlock->uValue.fTableField.fField = this->_ptr->uValue.fTableField.fField;
			pointerBlock->uValue.fTableField.fNbSubTables = this->_ptr->uValue.fTableField.fNbSubTables;
			
			memcpy(pointerBlock->uValue.fTableField.fSubTables,
				   this->_ptr->uValue.fTableField.fSubTables,
				   sizeof(this->_ptr->uValue.fTableField.fSubTables));
			
			break;
		default:
			break;			
	}
}

void C_POINTER::getVariable(PA_Variable *variable)
{
	
	PA_Variable v;

	switch (this->_ptrType)
	{
		case ePK_PointerToVariable:	
			
			v = PA_GetPointerValue(this->_ptr);
			//should Call PA_ClearVariable to clear the returned PA_Variable.
			//see 4DPluginAPI.h
			variable->fType		= v.fType;
			variable->fFiller	= 1;
			variable->uValue.fVariableDefinition.fTag = v.uValue.fVariableDefinition.fTag;
			variable->uValue.fVariableDefinition.fIndice = v.uValue.fVariableDefinition.fIndice;
			variable->uValue.fVariableDefinition.fType = v.uValue.fVariableDefinition.fType;
			
			memcpy(variable->uValue.fVariableDefinition.fName,
				   v.uValue.fVariableDefinition.fName,
				   sizeof(v.uValue.fVariableDefinition.fName));
			
			break;
			
		case ePK_PointerToField:
			
			variable->fType		= 3;
			variable->fFiller	= 1;
			variable->uValue.fTableFieldDefinition.fTableNumber = this->_ptr->uValue.fTableField.fTable;						
			variable->uValue.fTableFieldDefinition.fFieldNumber = this->_ptr->uValue.fTableField.fField;	
			variable->uValue.fTableFieldDefinition.fUnused1 = 0;
			
			break;
		default:
			break;		
	}
	
}

PA_VariableKind C_POINTER::getValueType()
{
	return this->_ptrValueType;
}

PA_PointerKind C_POINTER::getType()
{
	return this->_ptrType;
}

C_POINTER::C_POINTER() : _ptr(0), _ptrType(ePK_InvalidPointer), _ptrValueType(eVK_Undefined)
{
	
}

C_POINTER::~C_POINTER()
{ 
	
}
