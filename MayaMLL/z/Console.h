#pragma once

class Console final
{
	inline static std::streambuf* cinBuffer;
	inline static std::streambuf* coutBuffer;
	inline static std::streambuf* cerrBuffer;

	inline static std::fstream consoleInput;
	inline static std::fstream consoleOutput;
	inline static std::fstream consoleError;

public:

	static void MayaOutputWindow()
	{
		// redirect cout to cerr, so that when we do cout goes to cerr
		// in the maya output window (not the scripting output!)
		cout.set_rdbuf(MStreamUtils::stdOutStream().rdbuf());
		std::cerr.set_rdbuf(MStreamUtils::stdErrorStream().rdbuf());
	}

	static void Activate()
	{
		AllocConsole();

		cinBuffer = std::cin.rdbuf();
		coutBuffer = std::cout.rdbuf();
		cerrBuffer = std::cerr.rdbuf();

		consoleInput.open("CONIN$", std::ios::in);
		consoleOutput.open("CONOUT$", std::ios::out);
		consoleError.open("CONOUT$", std::ios::out);

		std::cin.rdbuf(consoleInput.rdbuf());
		std::cout.rdbuf(consoleOutput.rdbuf());
		std::cerr.rdbuf(consoleError.rdbuf());

		if (HMENU menu = GetSystemMenu(GetConsoleWindow(), FALSE))
		{
			EnableMenuItem(menu, SC_CLOSE, MF_GRAYED); // MF_DISABLED // MF_GRAYED
		}
	}

	static void Destroy()
	{
		consoleInput.close();
		consoleOutput.close();
		consoleError.close();

		std::cin.rdbuf(cinBuffer);
		std::cout.rdbuf(coutBuffer);
		std::cerr.rdbuf(cerrBuffer);

		cinBuffer = nullptr;
		coutBuffer = nullptr;
		cerrBuffer = nullptr;

		FreeConsole();
	}
};


