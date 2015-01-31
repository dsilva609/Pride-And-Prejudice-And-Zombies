#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

class FileParser
{
public:
	vector<string> Read(string filename)
	{
		vector<string> data;

		this->_stream.open(filename);

		if (this->_stream.good())
		{
			cout << "Reading contents of " << filename << "..." << endl;
			while (this->_stream.good())
			{
				getline(this->_stream, this->_lineIn);

				data.push_back(this->_lineIn);
			}
		}
		else
			cout << "ERROR: Could not open file: " << filename << endl;

		this->_stream.close();

		return data;
	}

	void Write(string filename, map<string, string> data)
	{
		this->_stream.open(filename);

		if (this->_stream.good())
		{
			cout << "Saving data to " << filename << "..." << endl;

			//	//this needs to be threaded
			for (map<string, string>::iterator i = data.begin(); i != data.end(); i++)
				this->_stream << i->first + " " + i->second << endl;
		}
		else
			cout << "ERROR: Could not open file: " << filename << endl;

		this->_stream.close();
	}

private:
	fstream _stream;
	string _lineIn;
	string _temp;

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