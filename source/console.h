#pragma once

#include "constants.h"
#include "textDX.h"
#include <vector>

class Console
{
private:
	TextDX dxLogFont;
	std::vector<std::string> logList;
	std::string logText;
	
public:
	// Constructor
	Console();
	// Destructor
	virtual ~Console();
	// set up DX
	void initialize(Graphics* graphics);


	// reset text
	void resetLog();
	// sets the next log output message
	void setLogText(int number);
	void setLogText(const Vec2Int vec);
	void setLogText(const std::string &message);
	// adds line to log array
	void AddLine(const std::string newMessage);
	// set and immediately display
	void renderLog(const std::string &message);
	// displays message to the screen
	void renderLog(); 


	// housekeeping
	void onLostDevice();
	void onResetDevice();

};