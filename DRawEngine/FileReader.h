#pragma once

#include <fstream>
#include <string>
#include <vector>

using namespace std;

class FileReader
{
public:

	vector<char>& Read(string& fileLocation);

	vector<char>& GetCached();

	void ClearCache();
private:
	vector<char> _cachedData;

};