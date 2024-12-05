#include "common_functions.h"

#include <cerrno>
#include <cstring>
#include <filesystem>
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

void CommonFunctions::printFileError(const std::string& fileName) {
	if (fileName.empty() || (0 == errno)) {
		return;
	}

	auto errorDescriptionPtr = std::strerror(errno);
	if (nullptr == errorDescriptionPtr) {
		std::cerr << "{CommonFunctions::printFileError}; "
			"error '" << errno << "' occurred during operations with "
			"file '" << fileName << "'" << std::endl;
		return;
	}

	auto descriptionLength = std::strlen(errorDescriptionPtr);
	std::string errorDescription(errorDescriptionPtr, descriptionLength);
	if (errorDescription.empty()) {
		std::cerr << "{CommonFunctions::printFileError}; "
			"error '" << errno << "' occurred during operations with "
			"file '" << fileName << "'" << std::endl;
		return;
	}

	std::cerr << "{CommonFunctions::printFileError}; "
		"error '" << errno << "' occurred during operations with "
		"file '" << fileName << "'; "
		"error description: '" << errorDescription << "'" << std::endl;
}

void CommonFunctions::printSemaphoreError() {
	if (0 == errno) {
		return;
	}

	auto errorDescriptionPtr = std::strerror(errno);
	if (nullptr == errorDescriptionPtr) {
		std::cerr << "{CommonFunctions::printSemaphoreError}; "
			"error '" << errno << "' occurred during operations with semaphore" << std::endl;
		return;
	}

	auto descriptionLength = std::strlen(errorDescriptionPtr);
	std::string errorDescription(errorDescriptionPtr, descriptionLength);
	if (errorDescription.empty()) {
		std::cerr << "{CommonFunctions::printSemaphoreError}; "
			"error '" << errno << "' occurred during operations with semaphore" << std::endl;
		return;
	}

	std::cerr << "{CommonFunctions::printSemaphoreError}; "
		"error '" << errno << "' occurred during operations with semaphore; "
		"error description: '" << errorDescription << "'" << std::endl;
}

bool CommonFunctions::fileExists(const std::string& fileName) {
    if (fileName.empty()) {
        std::cerr << "{CommonFunctions::fileExists}; file name is empty" << std::endl;
        return false;
    }
	if (!std::filesystem::exists(fileName)) {
        std::cerr << "{CommonFunctions::fileExists}; file '" << fileName << "' does NOT exist" << std::endl;
        return false;
    }
    return true;
}

bool CommonFunctions::isRegularFile(const std::string& fileName) {
    if (fileName.empty()) {
        std::cerr << "{CommonFunctions::isRegularFile}; file name is empty" << std::endl;
        return false;
    }
	if (!std::filesystem::is_regular_file(fileName)) {
        std::cerr << "{CommonFunctions::isRegularFile}; file '" << fileName << "' is NOT a regular file" << std::endl;
        return false;
    }
    return true;
}
