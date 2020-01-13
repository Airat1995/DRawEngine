#include "FileReader.h"

string* FileReader::Read(string* fileLocation)
{
	fstream fileStream(fileLocation->c_str(), std::ios_base::in);

	if (!fileStream.is_open())
	{	
		std::stringstream formattedString;
		formattedString << "failed to open file! file name is " << fileLocation;
		throw std::runtime_error(formattedString.str());
	}
	
	stringstream buffer;
	buffer << fileStream.rdbuf();
	fileStream.close();
	_cachedData = buffer.str();

	return &_cachedData;
}

string FileReader::GetCached()
{
	return _cachedData;
}

void FileReader::ClearCache()
{
	_cachedData = "";
}
