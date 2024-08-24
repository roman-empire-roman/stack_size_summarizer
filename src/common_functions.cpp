#include "common_functions.h"

#include <cerrno>
#include <cstring>
#include <iostream>

bool CommonFunctions::convertStringToULong(const std::string& input, unsigned long& output) {
	output = 0;
	if (input.empty()) {
		std::cerr << "{CommonFunctions::convertStringToULong}; input string is empty";
		return false;
	}

	try {
		output = std::stoul(input);
	}
	catch (const std::invalid_argument& exception) {
		std::cerr << "{CommonFunctions::convertStringToULong}; exception 'std::invalid_argument' was caught during conversion of string '" << input << "' to type 'unsigned long'; "
			"exception description: '" << exception.what() << "'" << std::endl;
		return false;
	}
	catch (const std::out_of_range& exception) {
		std::cerr << "{CommonFunctions::convertStringToULong}; exception 'std::out_of_range' was caught during conversion of string '" << input << "' to type 'unsigned long'; "
			"exception description: '" << exception.what() << "'" << std::endl;
		return false;
	}
	catch (...) {
		std::cerr << "{CommonFunctions::convertStringToULong}; unknown exception was caught during conversion of string '" << input << "' to type 'unsigned long'" << std::endl;
		return false;
	}
	return true;
}

void CommonFunctions::printError(const std::string& streamName) {
	if (streamName.empty() || (0 == errno)) {
		return;
	}

	auto errorDescriptionPtr = std::strerror(errno);
	if (nullptr == errorDescriptionPtr) {
		return;
	}

	auto descriptionLength = std::strlen(errorDescriptionPtr);
	std::string errorDescription(errorDescriptionPtr, descriptionLength);
	if (errorDescription.empty()) {
		return;
	}

	std::cerr << "{CommonFunctions::printError}; "
		"error '" << errno << "' occurred during operations with "
		"stream '" << streamName << "'; "
		"error description: '" << errorDescription << "'" << std::endl;
}
