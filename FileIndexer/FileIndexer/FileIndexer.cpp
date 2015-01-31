#include <map>
#include <sstream>
#include <thread>
#include "FileParser.cpp"

class FileIndexer
{
public:
	void Execute(string filename, int numThreads)
	{
		this->_numThreads = numThreads;
		//this->_data = this->_parser.Read(filename);
		//this->DetermineIndices(numThreads);
		//this->CreateThreads(numThreads);
		//	this->IndexData();
	}

private:
	FileParser _parser;
	vector<string> _data;
	int _numThreads;
	map<string, string> _dictionary;
	vector<thread> _threads;
	vector<int> _indices;

	void IndexData()
	{
		/* ALGORITHM
			-needs
			--start index
			--end index
			-start from start index
			--detemine index locations for each word in string
			--incement index per word not per line
			-sort ascending when finished

			-need to handle rounding for division
			*/

		//test
		string test = "It It is a truth universally acknowledged, that a single man in possession";

		string currentWord;
		int currentIndex = 0;
		stringstream ss;

		//for (int i = 0; i < this->_data.size(); i++)
		//{
		//ss.str(this->_data.at(i));

		ss.str(test);
		while (ss.good())
		{
			ss >> currentWord;
			cout << currentWord << endl;

			if (this->_dictionary.find(currentWord) != this->_dictionary.end())
				this->_dictionary[currentWord] += " 9";
			else
				this->_dictionary[currentWord] = "5";
			cout << this->_dictionary.find(currentWord)->second << endl;

			currentWord.clear();
			currentIndex++;
		}
		//}
	}

	void DetermineIndices()
	{
		int offset, endIndex;

		offset = this->_data.size() / this->_numThreads;
		endIndex = 0;

		for (int i = 0; i < this->_numThreads; i++)
		{
			endIndex += offset;
			this->_indices.push_back(endIndex - offset);
			this->_indices.push_back(endIndex);
			endIndex++;
		}
	}

	void CreateThreads()
	{
		for (int i = 0; i < this->_numThreads; i++)
		{
			this->_threads.push_back(thread(&FileIndexer::Created, this, i));
			this->_threads.at(i).join();
		}
	}

	void Created(int num)
	{
		cout << "created thread " << num << endl;
	}
};