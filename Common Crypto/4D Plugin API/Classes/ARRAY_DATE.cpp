/*
 *  ARRAY_DATE.cpp
 *  4D Plugin
 *
 *  Created by miyako on 31/10/14.
 *
 */

#include "ARRAY_DATE.h"

void ARRAY_DATE::fromParamAtIndex(PackagePtr pParams, uint32_t index)
{	
	if(index)		
	{
		PA_Variable arr = *((PA_Variable*) pParams[index - 1]);

		this->setSize(0);
		
		if(arr.fType == eVK_ArrayDate)
		{
			uint32_t i;
			
			PA_Date date;
			C_DATE dateValue;
            
			for(i = 0; i <= (unsigned int)arr.uValue.fArray.fNbElements; i++)
			{
                date.fDay   = 0;
                date.fMonth = 0;
                date.fYear  = 0;
                
                date = (* (PA_Date**) (arr.uValue.fArray.fData) )[i];
                
                dateValue.setYearMonthDay(date.fYear, date.fMonth, date.fDay);
                
				this->_CDateArray->push_back(dateValue);				
			}
			
		}
		
	}
			
}

void ARRAY_DATE::toParamAtIndex(PackagePtr pParams, uint32_t index)
{
	if(index)		
	{
		PA_Variable arr = *((PA_Variable*) pParams[index - 1]);
		PA_Variable *param = ((PA_Variable *)pParams[index - 1]);
		
		switch (arr.fType) 
		{
			case eVK_ArrayDate:
				break;
			case eVK_Undefined:
				PA_ClearVariable(&arr);
				arr = PA_CreateVariable(eVK_ArrayDate);
				param->fType = arr.fType;
				break;				
				
			default:
				break;
		}		
		
		if(arr.fType == eVK_ArrayDate)
		{
			
			PA_ResizeArray(&arr, this->_CDateArray->empty() ? 0 : (uint32_t)(this->_CDateArray->size() - 1));
			
			uint32_t i;
            uint16_t day, month, year;
            
			for(i = 0; i < this->_CDateArray->size(); i++)
			{
                day = 0;
                month = 0;
                year = 0;
                
                this->_CDateArray->at(i).getYearMonthDay(&year, &month, &day);
                
                PA_SetDateInArray(arr, i, day, month, year);
			}
			
			param->fFiller = 0;
			param->uValue.fArray.fCurrent = arr.uValue.fArray.fCurrent;
			param->uValue.fArray.fNbElements = arr.uValue.fArray.fNbElements;
			param->uValue.fArray.fData = arr.uValue.fArray.fData;	
			
		}
		
	}
				
}

void ARRAY_DATE::appendYearMonthDay(uint16_t year, uint16_t month, uint16_t day)
{
    C_DATE date;
    
    date.setYearMonthDay(year, month, day);
    
	this->_CDateArray->push_back(date);	
}

void ARRAY_DATE::setYearMonthDayAtIndex(uint16_t year, uint16_t month, uint16_t day, uint32_t index)
{
	if(index < this->_CDateArray->size())
	{
        C_DATE date;
        
        date.setYearMonthDay(year, month, day);
        
		std::vector<C_DATE>::iterator it = this->_CDateArray->begin();
		it += index;
		
		this->_CDateArray->insert(this->_CDateArray->erase(it), date);
	}
}

void ARRAY_DATE::getYearMonthDayAtIndex(uint16_t *pYear, uint16_t *pMonth, uint16_t *pDay, uint32_t index)
{    
	if(index < this->_CDateArray->size())
	{
        this->_CDateArray->at(index).getYearMonthDay(pYear, pMonth, pDay);
	}
}

uint32_t ARRAY_DATE::getSize()
{
	return (uint32_t)this->_CDateArray->size();
}

void ARRAY_DATE::setSize(uint32_t size)
{
	this->_CDateArray->resize(size);
}

ARRAY_DATE::ARRAY_DATE() : _CDateArray(new CDateArray)
{
	
}

ARRAY_DATE::~ARRAY_DATE()
{ 
	delete _CDateArray; 
}