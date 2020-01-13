#pragma once

#include <fstream>
#include <string>
#include <iterator>
#include <sstream>

using namespace std;

class FileReader
{
public:

	string* Read(string* fileLocation);

	string GetCached();

	void ClearCache();
private:
	string _cachedData;

};