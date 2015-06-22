/*
 *  ARRAY_DATE.h
 *  4D Plugin
 *
 *  Created by miyako on 31/10/14.
 *
 */

#ifndef __ARRAY_DATE_H__
#define __ARRAY_DATE_H__ 1

#include "4DPluginAPI.h"
#include "C_DATE.h"

class C_DATE;

#ifdef __cplusplus
extern "C" {
#endif

	typedef std::vector<C_DATE> CDateArray;	
	
class ARRAY_DATE
{

private:

	CDateArray* _CDateArray;
	
public:
 
	void fromParamAtIndex(PackagePtr pParams, uint32_t index);
	void toParamAtIndex(PackagePtr pParams, uint32_t index);	
	
	void appendYearMonthDay(uint16_t year, uint16_t month, uint16_t day);	
	
	void setYearMonthDayAtIndex(uint16_t year, uint16_t month, uint16_t day, uint32_t index);	
	void getYearMonthDayAtIndex(uint16_t *pYear, uint16_t *pMonth, uint16_t *pDay, uint32_t index);	
	
	uint32_t getSize();
	void setSize(uint32_t size);
	
	ARRAY_DATE();
	~ARRAY_DATE();

};

#ifdef __cplusplus
}
#endif

#endif