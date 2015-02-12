#include "FileIndexer.cpp"
using namespace std;

int main()
{
	cout << "Hello world" << endl;
	(new FileIndexer())->Execute("mobydick.txt", "Output.txt", "ExemptWordsSorted.txt", 5);

	return EXIT_SUCCESS;
}