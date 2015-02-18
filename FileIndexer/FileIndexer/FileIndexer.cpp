#include <map>
#include <sstream>
#include <thread>
#include <algorithm>
#include "FileParser.cpp"

class FileIndexer
{
public:
	void Execute(string inputFilename, string outputFilename, string exemptListFilename, int numThreads)
	{
		this->_numThreads = numThreads;
		this->_data = this->_parser.Read(inputFilename);
		if (exemptListFilename != "")
			this->_exclusionData = this->_parser.Read(exemptListFilename);
		this->CompileExclusionDictionary();
		this->DetermineIndices();
		this->CreateThreads();
		this->_parser.Write(outputFilename, this->_dictionary);

		this->_data.clear();
		this->_exclusionData.clear();
		this->_dictionary.clear();
		this->_exclusionDictionary.clear();
	}

private:
	FileParser _parser;
	vector<string> _data;
	vector<string> _exclusionData;
	int _numThreads;
	map<string, Index> _dictionary;
	map<string, string> _exclusionDictionary;
	vector<thread> _threads;
	vector<int> _indices;
	bool _writeLocked = false;
	int wordIndex = 0;
	int _numThreadsRunning = this->_numThreads;
	thread _waitThread;

	void IndexData(int threadID, int start, int end)
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
		int currentIndex = start;// this->_indices.at(threadID * 2);
		int endIndex = end;// this->_indices.at(threadID * 2 + 1);
		stringstream ss;

		if (endIndex > this->_data.size())
			endIndex = this->_data.size();

		for (int i = currentIndex; i < endIndex; i++)
		{
			ss.str(CleanString(this->_data.at(i)));

			while (ss.good())
			{
				ss >> currentWord;
				if (this->_exclusionDictionary.find(currentWord) == this->_exclusionDictionary.end())
					this->QueueWrite(threadID, currentWord, wordIndex);

				currentWord.clear();
				currentIndex++;
				wordIndex++;
			}
			ss.clear();
		}

		this->_numThreadsRunning--;
		cout << "thread " << threadID << " finished" << endl;
	}

	string CleanString(string str)
	{
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		replace_if(str.begin(), str.end(), not1(std::ptr_fun<int, int>(::isascii)), ' ');
		replace_if(str.begin(), str.end(), not1(std::ptr_fun<int, int>(::isalpha)), ' ');
		replace_if(str.begin(), str.end(), ::isblank, ' ');

		return str;
	}

	void CompileExclusionDictionary()
	{
		for (auto item : this->_exclusionData)
			this->_exclusionDictionary.insert(make_pair(item, item));
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
			if (i == this->_numThreads)
				endIndex = this->_data.size();
			else
				this->_indices.push_back(endIndex);
			endIndex++;
		}
	}

	void CreateThreads()
	{
		this->_waitThread = thread(&FileIndexer::WaitForThreadsToComplete, this);

		for (int i = 0; i < this->_numThreads; i++)
			this->_threads.push_back(thread(&FileIndexer::IndexData, this, i, this->_indices.at(i * 2), this->_indices.at(i * 2 + 1)));

		this->_waitThread.join();
	}

	void QueueWrite(int threadID, string key, int value)
	{
		while (this->_writeLocked)
			this->_writeLocked = false;

		this->WriteData(threadID, key, value);
	}

	void WriteData(int threadID, string key, int value)
	{
		this->_writeLocked = true;

		if (this->_dictionary.find(key) == this->_dictionary.end())
			this->_dictionary[key].key = key;

		this->_dictionary[key].values.insert(make_pair(value, value));

		this->_writeLocked = false;
	}

	void WaitForThreadsToComplete()
	{
		cout << "waiting for threads complete..." << endl;
		while (this->_numThreadsRunning > 0)
			this->_numThreadsRunning = this->_numThreadsRunning;

		cout << "all threads completed" << endl;
		//then join all threads including wait thread

		for (int i = 0; i < this->_numThreads; i++)
			this->_threads[i].join();
	}
};