#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

class FileParser
{
public:
	vector<string> Read(string filename)
	{
		fstream stream;
		string lineIn;
		string temp;
		vector<string> data;

		stream.open(filename);

		if (stream.good())
		{
			cout << "Reading contents of " << filename << "..." << endl;
			while (stream.good())
			{
				getline(stream, lineIn);

				data.push_back(lineIn);
			}
			stream.close();
		}
		else
			cout << "ERROR: Could not open file: " << filename << endl;

		return data;
	}
};