#include <map>
#include <sstream>
#include <thread>
#include "FileParser.cpp"

class FileIndexer
{
public:
	void Execute(string inputFilename, string outputFilename, int numThreads)
	{
		this->_numThreads = numThreads;
		this->_data = this->_parser.Read(inputFilename);
		this->DetermineIndices();
		this->CreateThreads();
		this->_parser.Write(outputFilename, this->_dictionary);
	}

private:
	FileParser _parser;
	vector<string> _data;
	int _numThreads;
	map<string, string> _dictionary;
	vector<thread> _threads;
	vector<int> _indices;
	bool _writeLocked = false;

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

		cout << "from thread: " << threadID << endl;
		string currentWord;
		int currentIndex = this->_indices.at(threadID * 2);
		int endIndex = this->_indices.at(threadID * 2 + 1);
		stringstream ss;

		if (endIndex > this->_data.size())
			endIndex = this->_data.size();

		for (int i = currentIndex; i < endIndex; i++)
		{
			ss.str(this->_data.at(i));
			//need lowercase
			//need to create regular expression to remove non characters and split into words

			while (ss.good())
			{
				ss >> currentWord;

				this->QueueWrite(threadID, currentWord, currentIndex);

				currentWord.clear();
				currentIndex++;
			}
			ss.clear();
		}

		cout << "thread " << threadID << " finished" << endl;
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

	void WriteData(int threadID, string key, int value)
	{
		this->_writeLocked = true;

		if (this->_dictionary.find(key) != this->_dictionary.end())
			this->_dictionary[key] += " " + to_string(value);
		else
			this->_dictionary[key] = to_string(value);

		this->_writeLocked = false;
	}

	void QueueWrite(int threadID, string key, int value)
	{
		while (this->_writeLocked)
			cout << "write locked: " << this->_writeLocked << endl;

		this->WriteData(threadID, key, value);
	}
};