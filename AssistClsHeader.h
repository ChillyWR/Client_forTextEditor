#pragma once
#include <fstream>
#include <sstream>
#include <string>

class TextRedactor {
protected:
	static constexpr char redactorFile[] = "redactor.txt";
	static constexpr int textRowsChar[2] = { 8, 128 };            //{ rows amount; characters in each row}
	static constexpr int textSize = textRowsChar[0] * textRowsChar[1];
	struct TextExceptions {
		static constexpr char InvalidChanges[] = "Error: invalid changes";
	};
private:
	std::fstream m_Text;
	char m_Buff[1028]{};
	inline bool FileExists() const;
public:
	TextRedactor();
	void writeText(char* newText);
	std::string getText();
};