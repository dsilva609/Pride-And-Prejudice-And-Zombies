#include <map>
#include <sstream>
#include <thread>
#include <algorithm>
#include "FileParser.cpp"

class FileIndexer
{
public:
	void Execute(string inputFilename, string outputFilename, int numThreads)
	{
		this->_numThreads = numThreads;
		this->_data = this->_parser.Read(inputFilename);
		this->_exclusionData = this->_parser.Read("ExclusionList.txt");
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
			ss.str(CleanString(this->_data.at(i)));
			//need lowercase
			//need to create regular expression to remove non characters and split into words

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

	void QueueWrite(int threadID, string key, int value)
	{
		while (this->_writeLocked)
			cout << "write locked: " << this->_writeLocked << endl;

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
};