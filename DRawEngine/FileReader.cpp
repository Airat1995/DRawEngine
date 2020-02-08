#include "FileReader.h"

vector<char>& FileReader::Read(string& fileLocation)
{
	std::ifstream infile(fileLocation, std::ios::binary);

	_cachedData = vector<char>(std::istreambuf_iterator<char>(infile),
		std::istreambuf_iterator<char>());

	return _cachedData;
}

vector<char>& FileReader::GetCached()
{
	return _cachedData;
}

void FileReader::ClearCache()
{
	_cachedData.clear();
}
