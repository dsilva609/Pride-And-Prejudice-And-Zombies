#include "FileIndexer.cpp"
using namespace std;

int main()
{
	cout << "Hello world" << endl;
	(new FileIndexer())->Execute("PrideAndPrejudice.txt", "Output.txt", 5);

	return EXIT_SUCCESS;
}