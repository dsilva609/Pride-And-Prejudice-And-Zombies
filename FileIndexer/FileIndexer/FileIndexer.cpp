#include "FileParser.cpp"

class FileIndexer
{
public:
	void Execute(string filename)
	{
		this->_data = this->_parser.Read(filename);
	}

private:
	vector<string> _data;
	FileParser _parser;
};