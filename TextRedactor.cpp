#include "AssistClsHeader.h"

TextRedactor::TextRedactor() {
	if (!FileExists()) {
		m_Text.open(redactorFile, std::ios::in | std::ios::out | std::ios::trunc);
		m_Text.close();
	}
}
bool TextRedactor::FileExists() const {
	return std::ifstream(redactorFile).good();
}
void TextRedactor::writeText(char* newText) {
	m_Text.open(redactorFile, std::ios::in | std::ios::out | std::ios::trunc);
	m_Text << newText;
	m_Text.close();
}
std::string TextRedactor::getText() {
	m_Text.open(redactorFile, std::ios::in || std::ios::app);
	std::stringstream ss;
	ss << m_Text.rdbuf();
	m_Text.close();
	return ss.str();
}