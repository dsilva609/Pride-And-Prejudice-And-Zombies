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
		this->DetermineIndices();
		this->CreateThreads();
		this->_parser.Write(outputFilename, this->_dictionary);
	}

private:
	FileParser _parser;
	vector<string> _data;
	int _numThreads;
	map<string, Index> _dictionary;
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
		Index index;

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
				index.key = currentWord;
				index.values.insert(make_pair(currentIndex, currentIndex));

				this->QueueWrite(threadID, index);

				currentWord.clear();
				currentIndex++;
				index.key.clear();
				index.values.clear();
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

	void QueueWrite(int threadID, Index index)
	{
		while (this->_writeLocked)
			cout << "write locked: " << this->_writeLocked << endl;

		this->WriteData(threadID, index);
	}

	void WriteData(int threadID, Index index)
	{
		map<int, int> temp;

		this->_writeLocked = true;

		if (this->_dictionary.find(index.key) != this->_dictionary.end())
		{
			temp = this->_dictionary[index.key].values;
			this->_dictionary.erase(index.key);
			temp.insert(make_pair(index.values[0], index.values[0]));
			//index.values = temp;
			this->_dictionary.insert(make_pair(index.key, index));
			//this->_dictionary[index.key].key = index.key;
			//this->_dictionary[index.key].values = temp;
			//this->_dictionary.find(index.key)->->second.values.insert(make_pair(index.values.at(0), index.values.at(0)));
			//this->_dictionary[index.key].values.insert(make_pair(index.values.at(0), index.values.at(0)));
			temp.clear();
		}
		else
			this->_dictionary[index.key] = index;

		this->_writeLocked = false;

		//	index.key.clear();
		//index.values.clear();
	}
};