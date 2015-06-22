/*
 *  ARRAY_TIME.h
 *  4D Plugin
 *
 *  Created by miyako on 31/10/14.
 *
 */

#ifndef __ARRAY_TIME_H__
#define __ARRAY_TIME_H__ 1

#include "4DPluginAPI.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef std::vector<int> CIntArray;	
	
class ARRAY_TIME
{

private:

	CIntArray* _CIntArray;
	
public:
 
	void fromParamAtIndex(PackagePtr pParams, uint32_t index);
	void toParamAtIndex(PackagePtr pParams, uint32_t index);	
	
	void appendTimeValue(int intValue);	
	
	void setTimeValueAtIndex(int intValue, uint32_t index);	
	int getTimeValueAtIndex(uint32_t index);	
	
	uint32_t getSize();
	void setSize(uint32_t size);
	
	ARRAY_TIME();
	~ARRAY_TIME();

};

#ifdef __cplusplus
}
#endif

#endif