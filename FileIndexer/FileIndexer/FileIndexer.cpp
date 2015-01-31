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
		//this->DetermineIndices();
		this->CreateThreads();
	}

private:
	FileParser _parser;
	vector<string> _data;
	int _numThreads;
	map<string, string> _dictionary;
	vector<thread> _threads;
	vector<int> _indices;

	void IndexData(int threadID)
	{
		/* ALGORITHM
			-needs
			--start index
			--end index
			-start from start index
			--detemine index locations for each word in string
			--incement index per word not per line

			-map needs to be sort ascending when finished
			-need to handle singluar write to map for threads
			-need to handle rounding for division
			*/

		//test
		//string test = "It It is a truth universally acknowledged, that a single man in possession";


		cout << "from thread: " << threadID << endl;
		string currentWord;
		int currentIndex = this->_indices.at(threadID * 2);
		int endIndex = this->_indices.at(threadID * 2 + 1);
		stringstream ss;

		for (int i = currentIndex; i < endIndex; i++)
		{
			ss.str(this->_data.at(i));

			//ss.str(test);
			while (ss.good())
			{
				ss >> currentWord;
				//	cout << currentWord << endl;

				if (this->_dictionary.find(currentWord) != this->_dictionary.end())
					this->_dictionary[currentWord] += " " + to_string(currentIndex);
				else
					this->_dictionary[currentWord] = to_string(currentIndex);
				//cout << this->_dictionary.find(currentWord)->second << endl;

				currentWord.clear();
				currentIndex++;
			}
			ss.clear();
		}
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
			this->_threads.push_back(thread(&FileIndexer::IndexData, this, i));
			this->_threads.at(i).join();
		}
	}
};