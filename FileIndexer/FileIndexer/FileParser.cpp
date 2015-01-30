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

/* METHOD OF EXECUTION
	-read file
	--read once to get length
	--read a second time with threads to get lines at specific points
	-send data to indexer
	--send number of threads to be created
	--set one thread to index at one range
	--set another for another range
	--save indexes to dictionary? vector of objects with: string name and int[] of locations
	---create logic for accessing vector contents one thread at a time
	---remove punctuation
	*/