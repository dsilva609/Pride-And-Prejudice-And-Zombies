#include "FileIndexer.cpp"
using namespace std;

int main()
{
	cout << "Hello world" << endl;
	(new FileIndexer())->Execute("PrideAndPrejudice.txt", 1);

	return EXIT_SUCCESS;
}