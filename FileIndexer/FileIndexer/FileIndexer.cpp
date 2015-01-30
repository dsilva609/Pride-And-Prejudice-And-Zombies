#include <map>
#include <sstream>
#include "FileParser.cpp"

class FileIndexer
{
public:
	void Execute(string filename)
	{
		//	this->_data = this->_parser.Read(filename);
		this->IndexData();
	}

private:
	vector<string> _data;
	FileParser _parser;
	map<string, string> _dictionary;

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
};