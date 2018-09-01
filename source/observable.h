#pragma once


#include "collectdisplay.h"


/***********************************************************************/

class Observable {
	
public:
	virtual void registerDisplay(Display*) = 0;
	virtual void removeDisplay(Display*) = 0;
	virtual void notifyDisplays() = 0;	
	virtual ~Observable() {}
};

/***********************************************************************/


