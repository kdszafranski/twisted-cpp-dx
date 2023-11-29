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
	void log(int number);
	void log(const Vec2Int vec);
	void log(const std::string &message, const Vec2Int vec);
	void log(const std::string &message);

	// Adds log line to the array for display
	void AddLine(const std::string newMessage);
	
	// displays message to the screen
	void renderLog(); 


	// housekeeping
	void onLostDevice();
	void onResetDevice();

};