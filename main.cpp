#include<iostream>
#include<vector>
#include<fstream>
#include<string>
namespace st {
	using std::cout, std::cin, std::vector, std::ostream, std::istream, std::endl, std::ofstream, std::ifstream, std::string;
};

using namespace st;

class ICellRead {
	int virtual Read() = 0;
};
class ICellWrite {
public:
	void virtual Write(int input) = 0;
};

class Cell : public ICellWrite, ICellRead {
public:
	Cell(int def = 0) {
		content = def;
	}
	void Write(int input) override {
		content = input;
	}
	int Read() override {
		return content;
	}
private:
	int content;
};

class  SudokuDatabase : public Cell {
public:
	SudokuDatabase(int def = 0) : Cell(def) {
		Cell c;
		vector<Cell> v;
		for (int j = 0; j < 9; j++)
		{
			for (int i = 0; i < 9; i++)
			{
				v.push_back(c);
			}
			matrix.push_back(v);
		}
	}
	vector<vector<Cell>> GetMatrix()
	{
		return matrix;
	}
	void WriteCell(int PosX, int PosY, int input)
	{
		matrix[PosY][PosX].Write(input);
	}
	void SaveToFile()
	{
		ofstream outFile("Save.txt");
		if (outFile.is_open())
		{
			for (auto& row : matrix)
			{
				row.erase(row.begin() + 8, row.end() - 1);
				for (auto& cell : row)
				{

					outFile << cell.Read();
				}
				outFile.clear();
				outFile << "\n";
			}
			outFile.close();
			//cout << "Sudoku saved " << endl;
		}
		else {
			cout << "ERROR: Could not open file 'Save.txt' for writing.\n";
		}
	}
	void LoadFromFile()
	{
		ifstream outFile("Save.txt");
		if (outFile.is_open())
		{
			string LoadedData;
			for (int i = 0; i < 9; i++)
			{
				outFile >> LoadedData;
				for (int j = 0; j < 9; j++)
				{
					matrix[i][j].Write(std::stoi(string(1, LoadedData[j])));
					//outFile.get();
				}
			}
		}
		else
		{
			cout << "ERROR: Could not open file 'Save.txt' for loading.\n";
		}
	}

private:
	vector<vector<Cell>> matrix;
};

//overloading exit operator to quickly display the sudoku in a pleasing manner 
ostream& operator<<(ostream& out, SudokuDatabase& sdb)
{
	auto pom = sdb.GetMatrix();
	out << "      1   2   3   4   5   6   7   8   9   ";
	for (int i = 0; i < 9; i++)
	{
		if (i == 0)
			out << "\n    ,-----------,-----------,-----------,\n";
		else if (i % 3 == 0 && i != 0)
			out << "\n    |-----------|-----------|-----------|\n";
		else
			out << "\n    | - - - - - | - - - - - | - - - - - |\n";
		out << " " << i + 1 << " ";
		for (int j = 0; j < 9; j++)
		{
			if (j % 3 == 0)
				out << " | " << pom[i][j].Read();
			else
				out << " : " << pom[i][j].Read();
		}
		out << " :";
	}
	out << "\n    '-----------'-----------'-----------'\n";
	return out;
}

//Brief description of the rules regarding sudoku
void DaRules() {
	cout << "In Sudoku, the objective is to fill a 9 x 9 grid with digits so that each column, each row, and each of the nine 3 x 3 subgrids that compose the grid ";
	cout << "contains all of the digits from 1 to 9.\n";
}

//Checks if there are any digits that appear at least twice in vector v consistic solely of digits
//(vector v must have exactly 9 elements to work correctly this isn't an issue due to the fact that this function is used in sudoku)
bool counter(vector<Cell> v)
{
	;

	for (int i = 1; i < 10; i++)
	{
		int count = 0;
		for (int j = 0; j < 9; j++)
		{
			if (v[j].Read() == i)
				count++;
			if (count > 1)
				return true;
			//true <-> there is a duplicate in the vector
		}
	}
	return false;
	//false <-> there is not a single duplicate in the vector
}

//Checks if there is an error in  rows
bool ERRORRow(SudokuDatabase& sdb) {
	bool out = false;
	for (int i = 0; i < 9; i++)
	{
		auto pom = sdb.GetMatrix()[i];
		if (counter(pom))
			return true;
	}
	return false;
}

//Checks if there is an error in collumns 
bool ERRORCol(SudokuDatabase& sdb) {
	bool out = false;
	vector<Cell> p;
	for (int i = 0; i < 9; i++)
	{
		p.clear();
		for (int j = 0; j < 9; j++)
		{
			//matrix[PosY][PosX].Write(input);
			p.push_back(sdb.GetMatrix()[j][i]);
		}
		if (counter(p))
			return true;
	}
	return false;
}

//Checks if there is an error in 3x3 square
bool ERRORSqu(SudokuDatabase& sdb) {
	bool out = false;
	vector<Cell> p;
	for (int l = 0; l < 3; l++)
	{
		for (int k = 0; k < 3; k++)
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					p.push_back(sdb.GetMatrix()[i + 3 * k][j + 3 * l]);

					//cout << "%%---j + 3 * l :" << j + 3 * l << endl;
				}
				//cout << "&&---i + 3 * k :" << i + 3 * k << endl;
			}
			if (counter(p))
				return true;
			p.clear();
		}
	}
	return false;
}

bool Win(SudokuDatabase& sdb)
{
	bool out = true;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (sdb.GetMatrix()[i][j].Read() == 0)
				out = false;
		}
	}
	if (ERRORCol(sdb) || ERRORRow(sdb) || ERRORSqu(sdb))
		out = false;
	return out;
}

bool TestALL(SudokuDatabase& sdb)
{
	return (ERRORCol(sdb) || ERRORRow(sdb) || ERRORSqu(sdb));
}

SudokuDatabase Start(int difficulty, SudokuDatabase& sdb)
{
	//Number of Digits Left
	int NoDL = 17 * (5 - difficulty);

	//Template ie a sudoku that's been done
	// 
	//Row 1
	{
		sdb.WriteCell(0, 0, 5);
		sdb.WriteCell(1, 0, 3);
		sdb.WriteCell(2, 0, 4);
		sdb.WriteCell(3, 0, 6);
		sdb.WriteCell(4, 0, 7);
		sdb.WriteCell(5, 0, 8);
		sdb.WriteCell(6, 0, 9);
		sdb.WriteCell(7, 0, 1);
		sdb.WriteCell(8, 0, 2);
	}
	//Row 2
	{
		sdb.WriteCell(0, 1, 6);
		sdb.WriteCell(1, 1, 7);
		sdb.WriteCell(2, 1, 2);
		sdb.WriteCell(3, 1, 1);
		sdb.WriteCell(4, 1, 9);
		sdb.WriteCell(5, 1, 5);
		sdb.WriteCell(6, 1, 3);
		sdb.WriteCell(7, 1, 4);
		sdb.WriteCell(8, 1, 8);
	}
	//Row 3
	{
		sdb.WriteCell(0, 2, 1);
		sdb.WriteCell(1, 2, 9);
		sdb.WriteCell(2, 2, 8);
		sdb.WriteCell(3, 2, 3);
		sdb.WriteCell(4, 2, 4);
		sdb.WriteCell(5, 2, 2);
		sdb.WriteCell(6, 2, 5);
		sdb.WriteCell(7, 2, 6);
		sdb.WriteCell(8, 2, 7);
	}
	//Row 4
	{
		sdb.WriteCell(0, 3, 8);
		sdb.WriteCell(1, 3, 5);
		sdb.WriteCell(2, 3, 9);
		sdb.WriteCell(3, 3, 7);
		sdb.WriteCell(4, 3, 6);
		sdb.WriteCell(5, 3, 1);
		sdb.WriteCell(6, 3, 4);
		sdb.WriteCell(7, 3, 2);
		sdb.WriteCell(8, 3, 3);
	}
	//Row 5
	{
		sdb.WriteCell(0, 4, 4);
		sdb.WriteCell(1, 4, 2);
		sdb.WriteCell(2, 4, 6);
		sdb.WriteCell(3, 4, 8);
		sdb.WriteCell(4, 4, 5);
		sdb.WriteCell(5, 4, 3);
		sdb.WriteCell(6, 4, 7);
		sdb.WriteCell(7, 4, 9);
		sdb.WriteCell(8, 4, 1);
	}
	//Row 6
	{
		sdb.WriteCell(0, 5, 7);
		sdb.WriteCell(1, 5, 1);
		sdb.WriteCell(2, 5, 3);
		sdb.WriteCell(3, 5, 9);
		sdb.WriteCell(4, 5, 2);
		sdb.WriteCell(5, 5, 4);
		sdb.WriteCell(6, 5, 8);
		sdb.WriteCell(7, 5, 5);
		sdb.WriteCell(8, 5, 6);
	}
	//Row 7
	{
		sdb.WriteCell(0, 6, 9);
		sdb.WriteCell(1, 6, 6);
		sdb.WriteCell(2, 6, 1);
		sdb.WriteCell(3, 6, 5);
		sdb.WriteCell(4, 6, 3);
		sdb.WriteCell(5, 6, 7);
		sdb.WriteCell(6, 6, 2);
		sdb.WriteCell(7, 6, 8);
		sdb.WriteCell(8, 6, 4);
	}
	//Row 8
	{
		sdb.WriteCell(0, 7, 2);
		sdb.WriteCell(1, 7, 8);
		sdb.WriteCell(2, 7, 7);
		sdb.WriteCell(3, 7, 4);
		sdb.WriteCell(4, 7, 1);
		sdb.WriteCell(5, 7, 9);
		sdb.WriteCell(6, 7, 6);
		sdb.WriteCell(7, 7, 3);
		sdb.WriteCell(8, 7, 5);
	}
	//Row 9
	{
		sdb.WriteCell(0, 8, 3);
		sdb.WriteCell(1, 8, 4);
		sdb.WriteCell(2, 8, 5);
		sdb.WriteCell(3, 8, 2);
		sdb.WriteCell(4, 8, 8);
		sdb.WriteCell(5, 8, 6);
		sdb.WriteCell(6, 8, 1);
		sdb.WriteCell(7, 8, 7);
		sdb.WriteCell(8, 8, 9);
	}
	srand(time(0));
	for (int i = 0; i < 81 - NoDL; i++)
	{
		int X = 0, Y = 0, digit = 0;
		X = rand() % 9;
		Y = rand() % 9;
		if (sdb.GetMatrix()[Y][X].Read() != 0)
			sdb.WriteCell(X, Y, 0);
		else
			i--;
	}
	cout << "Remainder: The game saves after each valid input.\n";
	return sdb;
}

int main()
{
	int difficulty = 1;
	SudokuDatabase sdb;
	bool end = true;
	while (end)
	{
		static int begin = 0;
		int choice = -1;
		if (begin == 0)
		{
			cout << "Type 0 in order to check the rules of Sudoku.\n";
			cout << "Type 1 in order to start the game.\n";
			cout << "Type 2 to resume.\n";
			cout << "Type 3 to exit the app.\n";
			try {
				cin >> choice;

				if (cin.fail()) {
					throw "Cin_ERROR";
				}
			}
			catch (...) {
				cout << "ERROR - INCORRECT INPUT\n";
				cin.clear();
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}

			switch (choice) {
			case 0:
				DaRules();
				break;
			case 1:
				cout << "Type the difficulty:\nEasy - type 1\nMedium - type 2\nHard - type 3\nImpossible - type 4\n";

				try
				{
					cin >> difficulty;
					if (cin.fail())
						throw("Cin_ERROR");
				}
				catch (...) {
					cout << "ERROR - INCORRECT INPUT\n";
					cin.clear();
					cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					continue;
				}

				sdb = Start(difficulty, sdb);
				begin = 1;
				cout << "Starting the game... \n";
				break;
			case 2:
				sdb.LoadFromFile();
				cout << "Sudoku's last session has been resumed\n";
				begin = 1;
				break;
			case 3:
				end = 0;
				break;
			default:
				break;
			}
		}
		if (begin == 1)
		{
			cout << sdb;
			int PositionX = 0, PositionY = 0, Digit = 0;

			cout << "Type X coordinate.\n";
			try
			{
				cin >> PositionX;
				if (cin.fail())
					throw("Cin_ERROR");
			}
			catch (...)
			{
				cout << "ERROR - INCORRECT INPUT\n";
				cin.clear();
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}
			PositionX--;

			cout << "Type Y coordinate.\n";
			try
			{
				cin >> PositionY;
				if (cin.fail())
					throw("Cin_ERROR");
			}
			catch (...)
			{
				cout << "ERROR - INCORRECT INPUT\n";
				cin.clear();
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}
			PositionY--;

			cout << "Type what digit to put in that coordinate.\n";
			try
			{
				cin >> Digit;
				if (cin.fail())
					throw("Cin_ERROR");
			}
			catch (...)
			{
				cout << "ERROR - INCORRECT INPUT\n";
				cin.clear();
				cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				continue;
			}

			sdb.WriteCell(PositionX, PositionY, Digit);
			sdb.SaveToFile();

			cout << "Is there an error in rows?\n";
			cout << std::boolalpha << ERRORRow(sdb) << endl;
			cout << "Is there an error in collumns?\n";
			cout << std::boolalpha << ERRORCol(sdb) << endl;
			cout << "Is there an error in 3x3 squares?\n";

			cout << std::boolalpha << ERRORSqu(sdb) << endl;
			if (Win(sdb))
			{
				cout << "Congratulations! You've successfully completed this sudoku.\n";
				begin = 0;
			}
		}
	}
	return 0;
}