
#include "console.h"
#include <sstream>

Console::Console()
{
	logText = "";
	logList.clear();
}

Console::~Console()
{
	onLostDevice();
}

void Console::onLostDevice()
{
	dxLogFont.onLostDevice();
}

void Console::onResetDevice()
{
	dxLogFont.onResetDevice();
}

void Console::initialize(Graphics *graphics)
{
	// Init DirectX font with 48px high Arial
	if (dxLogFont.initialize(graphics, 16, true, false, "Arial") == false)
		throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing DirectX font"));

}

void Console::log(const int number)
{
	logText = std::to_string(number);
	AddLine(logText);
}
void Console::log(const Vec2Int vec)
{
	std::stringstream ss;
	ss << "(" << vec.x << ", " << vec.y << ")";
	logText = ss.str();
	AddLine(logText);
}
void Console::log(const std::string& message, const Vec2Int vec)
{
	std::stringstream ss;
	ss << message << "  (" << vec.x << ", " << vec.y << ")";
	logText = ss.str();
	AddLine(logText);
}
void Console::log(const std::string &message)
{
	logText = message;
	AddLine(logText);
}

// Adds log line to the array for display
void Console::AddLine(const std::string newMessage)
{
	logList.push_back(newMessage);
}

/// <summary>
/// Resets log text to empty string
/// </summary>
void Console::resetLog()
{
	logText = "";
	logList.clear();
}

/// <summary>
/// Renders messages to screen. Must be called between spriteBegin and spriteEnd
/// Called from game.render()
/// </summary>
void Console::renderLog()
{
	//if (logText.length() > 0) {
	//	// draw the text
	//	dxLogFont.setFontColor(graphicsNS::WHITE);
	//	dxLogFont.print(logText, 6, GAME_HEIGHT - 20);
	//}

	int yOffset = 20;
	for (int i = logList.size() - 1; i >= 0; i--) {
		// draw the text
		dxLogFont.setFontColor(graphicsNS::WHITE);
		dxLogFont.print(logList.at(i), 6, GAME_HEIGHT - yOffset * (logList.size() - i));
	}
}