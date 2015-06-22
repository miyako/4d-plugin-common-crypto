/*
 *  ARRAY_TIME.cpp
 *  4D Plugin
 *
 *  Created by miyako on 31/10/14.
 *
 */

#include "ARRAY_TIME.h"

void ARRAY_TIME::fromParamAtIndex(PackagePtr pParams, uint32_t index)
{	
	if(index)		
	{
		PA_Variable arr = *((PA_Variable*) pParams[index - 1]);

		this->setSize(0);
		
		if(arr.fType == eVK_ArrayTime)
		{
			uint32_t i;
			
			int intValue;
			
			for(i = 0; i <= (unsigned int)arr.uValue.fArray.fNbElements; i++)
			{
				intValue = ( * (int**) (arr.uValue.fArray.fData) )[i];
				this->_CIntArray->push_back(intValue);				
			}
			
		}
		
	}
			
}

void ARRAY_TIME::toParamAtIndex(PackagePtr pParams, uint32_t index)
{
	if(index)		
	{
		PA_Variable arr = *((PA_Variable*) pParams[index - 1]);
		PA_Variable *param = ((PA_Variable *)pParams[index - 1]);
		
		switch (arr.fType) 
		{
			case eVK_ArrayTime:
				break;
			case eVK_Undefined:
				PA_ClearVariable(&arr);
				arr = PA_CreateVariable(eVK_ArrayTime);
				param->fType = arr.fType;
				break;				
				
			default:
				break;
		}		
		
		if(arr.fType == eVK_ArrayTime)
		{
			
			PA_ResizeArray(&arr, this->_CIntArray->empty() ? 0 : (uint32_t)(this->_CIntArray->size() - 1));
			
			uint32_t i;
			
			for(i = 0; i < this->_CIntArray->size(); i++)
			{
				PA_SetTimeInArray(arr, i, this->_CIntArray->at(i));	
			}
			
			param->fFiller = 0;
			param->uValue.fArray.fCurrent = arr.uValue.fArray.fCurrent;
			param->uValue.fArray.fNbElements = arr.uValue.fArray.fNbElements;
			param->uValue.fArray.fData = arr.uValue.fArray.fData;	
			
		}
		
	}
				
}

void ARRAY_TIME::appendTimeValue(int intValue)
{
	this->_CIntArray->push_back(intValue);	
}

void ARRAY_TIME::setTimeValueAtIndex(int intValue, uint32_t index)
{
	if(index < this->_CIntArray->size())
	{
		
		std::vector<int>::iterator it = this->_CIntArray->begin();
		it += index;
		
		this->_CIntArray->insert(this->_CIntArray->erase(it), intValue);
		
	}
}

int ARRAY_TIME::getTimeValueAtIndex(uint32_t index)	
{
	
	int intValue = 0;
	
	if(index < this->_CIntArray->size())
	{
		
		intValue = this->_CIntArray->at(index);
	}
	
	return intValue;
	
}

uint32_t ARRAY_TIME::getSize()
{
	return (uint32_t)this->_CIntArray->size();
}

void ARRAY_TIME::setSize(uint32_t size)
{
	this->_CIntArray->resize(size);
}

ARRAY_TIME::ARRAY_TIME() : _CIntArray(new CIntArray)
{
	
}

ARRAY_TIME::~ARRAY_TIME()
{ 
	delete _CIntArray; 
}