#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <numeric>
#include <random>
#include <unordered_map>

constexpr unsigned int SudokuX = 9;
constexpr unsigned int SudokuY = 9;
constexpr unsigned int PositionOfUsedSudoku = 0;
constexpr unsigned int ColumnsHorizontal = 3;
constexpr unsigned int ColumnsVertical = 3;
//#define _DEBUGTEST

unsigned short Sudoku[SudokuX][SudokuY];
std::vector<short> PossibleNumbers[SudokuX][SudokuY];

unsigned int usedNumbers = 0;
bool sudokuSolved = false;

void LoadFile(const std::string& file)
{
    std::ifstream f;
    f.open(file, std::ios::binary);
    std::vector<unsigned short> sudokuNumbers;
    if (f.is_open())
    {
        for (std::string line; std::getline(f, line);)
        {
            for (char ch : line)
            {
                if (!std::isspace(ch))
                {
                    sudokuNumbers.push_back(std::atoi(&ch));
                }
            }
        }
    }
    for (int i = 0; i < SudokuX; i++)
        for (int j = 0; j < SudokuY; j++)
        {
            auto number = sudokuNumbers[PositionOfUsedSudoku * SudokuY * SudokuX + i * SudokuX + j];
            Sudoku[i][j] = number;
            if (number)
                usedNumbers++;
        }
    f.close();
}

void PrintSudoku()
{
    for (int i = 0; i < SudokuX; i++)
    {
        for (int j = 0; j < SudokuY; j++)
        {
            std::cout << Sudoku[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

bool CheckSudoku();
void PrintPossibleNumbers();

void FilterWrittenNumbers()
{
    for (unsigned i = 0; i < SudokuX; i++) {
        for (unsigned j = 0; j < SudokuY; j++) {
            if (!Sudoku[i][j])
            {
                PossibleNumbers[i][j] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

                for (unsigned k = 0; k < SudokuY; k++) {
                    if (Sudoku[i][k]) {
                        auto foundNum = std::find(PossibleNumbers[i][j].begin(), PossibleNumbers[i][j].end(),
                                                  Sudoku[i][k]);
                        if (foundNum != PossibleNumbers[i][j].end()) {
                            std::swap(*foundNum, *(PossibleNumbers[i][j].end() - 1));
                            PossibleNumbers[i][j].pop_back();
                        }
                    }
                    if (Sudoku[k][j]) {
                        auto foundNum = std::find(PossibleNumbers[i][j].begin(), PossibleNumbers[i][j].end(),
                                                  Sudoku[k][j]);
                        if (foundNum != PossibleNumbers[i][j].end()) {
                            std::swap(*foundNum, *(PossibleNumbers[i][j].end() - 1));
                            PossibleNumbers[i][j].pop_back();
                        }
                    }
                }
                unsigned short row = i / ColumnsVertical;
                unsigned short column = j / ColumnsHorizontal;
                for (unsigned k = row * ColumnsVertical; k < row * ColumnsVertical + ColumnsVertical; k++)
                {
                    for (unsigned l = column * ColumnsHorizontal; l < column * ColumnsHorizontal + ColumnsHorizontal; l++)
                    {
                        if (Sudoku[k][l])
                        {
                            auto foundNum = std::find(PossibleNumbers[i][j].begin(), PossibleNumbers[i][j].end(),
                                                      Sudoku[k][l]);
                            if (foundNum != PossibleNumbers[i][j].end()) {
                                std::swap(*foundNum, *(PossibleNumbers[i][j].end() - 1));
                                PossibleNumbers[i][j].pop_back();
                            }
                        }
                    }
                }
            }
        }
    }

}

void FilterSudoku() {

    FilterWrittenNumbers();
    for (unsigned i = 0; i < SudokuX; i++)
    {
        for (unsigned j = 0; j < SudokuY; j++)
        {
            if (Sudoku[i][j])
                continue;
            // check linear numbers
            for (unsigned k = 0; k < PossibleNumbers[i][j].size(); k++)
            {
                unsigned rowToCheck = i / ColumnsVertical;
                unsigned columnToCheck = j / ColumnsHorizontal;
                // check in row
                unsigned numberOfFoundInRow = 0;
                for (unsigned rowCheck = rowToCheck * ColumnsVertical; rowCheck < rowToCheck * ColumnsVertical + ColumnsVertical; rowCheck++)
                {
                    if (!Sudoku[rowCheck][j] && !PossibleNumbers[rowCheck][j].empty())
                    {
                        auto foundNum = std::find(PossibleNumbers[rowCheck][j].begin(), PossibleNumbers[rowCheck][j].end(),
                                                  PossibleNumbers[i][j][k]);
                        if (foundNum != PossibleNumbers[rowCheck][j].end()) {
                            numberOfFoundInRow++;
                        }
                    }
                }

                if (numberOfFoundInRow >= 2)
                {
                    bool canDeleteOthers = true;
                    for (unsigned rowCheck = rowToCheck * ColumnsVertical; rowCheck < rowToCheck * ColumnsVertical + ColumnsVertical && canDeleteOthers; rowCheck++)
                    {
                        for (unsigned columnCheck = columnToCheck * ColumnsHorizontal; columnCheck < columnToCheck * ColumnsHorizontal + ColumnsHorizontal; columnCheck++)
                        {
                            if (columnCheck == j)
                                continue;

                            auto numberFind = find(PossibleNumbers[rowCheck][columnCheck].begin(), PossibleNumbers[rowCheck][columnCheck].end(), PossibleNumbers[i][j][k]);
                            if (numberFind != PossibleNumbers[rowCheck][columnCheck].end())
                            {
                                canDeleteOthers = false;
                                break;
                            }

                        }
                    }


                    for (unsigned int rowToDelete = 0; rowToDelete < SudokuX && canDeleteOthers; rowToDelete++)
                    {
                        if (rowToDelete >= rowToCheck * ColumnsVertical && rowToDelete < rowToCheck * ColumnsVertical + ColumnsVertical)
                            continue;

                        auto foundNum = std::find(PossibleNumbers[rowToDelete][j].begin(), PossibleNumbers[rowToDelete][j].end(),
                                                  PossibleNumbers[i][j][k]);
                        if (foundNum != PossibleNumbers[rowToDelete][j].end()) {
                            std::swap(*foundNum, *(PossibleNumbers[rowToDelete][j].end() - 1));
                            PossibleNumbers[rowToDelete][j].pop_back();
                        }
                    }
                }

                // check in column
                unsigned numberOfFoundInColumn = 1;
                for (unsigned columnCheck = columnToCheck * ColumnsHorizontal; columnCheck < columnToCheck * ColumnsHorizontal + ColumnsHorizontal; columnCheck++)
                {
                    if (columnCheck != j && !Sudoku[i][columnCheck] && !PossibleNumbers[i][columnCheck].empty())
                    {
                        auto foundNum = std::find(PossibleNumbers[i][columnCheck].begin(), PossibleNumbers[i][columnCheck].end(),
                                                  PossibleNumbers[i][j][k]);
                        if (foundNum != PossibleNumbers[i][columnCheck].end()) {
                            numberOfFoundInColumn++;
                        }
                    }
                }

                if (numberOfFoundInColumn >= 2)
                {
                    bool canDeleteOthers = true;
                    for (unsigned rowCheck = rowToCheck * ColumnsVertical; rowCheck < rowToCheck * ColumnsVertical + ColumnsVertical && canDeleteOthers; rowCheck++)
                    {
                        for (unsigned columnCheck = columnToCheck * ColumnsHorizontal; columnCheck < columnToCheck * ColumnsHorizontal + ColumnsHorizontal; columnCheck++)
                        {
                            if (rowCheck == i)
                                continue;

                            auto numberFind = find(PossibleNumbers[rowCheck][columnCheck].begin(), PossibleNumbers[rowCheck][columnCheck].end(), PossibleNumbers[i][j][k]);
                            if (numberFind != PossibleNumbers[rowCheck][columnCheck].end())
                            {
                                canDeleteOthers = false;
                                break;
                            }

                        }
                    }

                    for (unsigned int columnToDelete = 0; columnToDelete < SudokuX && canDeleteOthers; columnToDelete++)
                    {
                        if (columnToDelete >= columnToCheck * ColumnsHorizontal && columnToDelete < columnToCheck * ColumnsHorizontal + ColumnsHorizontal)
                            continue;

                        auto foundNum = std::find(PossibleNumbers[i][columnToDelete].begin(), PossibleNumbers[i][columnToDelete].end(),
                                                  PossibleNumbers[i][j][k]);
                        if (foundNum != PossibleNumbers[i][columnToDelete].end()) {
                            std::swap(*foundNum, *(PossibleNumbers[i][columnToDelete].end() - 1));
                            PossibleNumbers[i][columnToDelete].pop_back();
                        }
                    }
                }
            }
        }
    }

    bool instantNumbers = false;
    for (int i = 0; i < SudokuX; i++)
    {
        for (int j = 0; j < SudokuY; j++)
        {
            if (!Sudoku[i][j] && PossibleNumbers[i][j].size() == 1)
            {
#ifdef _DEBUGTEST
                PrintSudoku();
#endif
                Sudoku[i][j] = PossibleNumbers[i][j][0];
                std::cout << "Writing raw number: " << PossibleNumbers[i][j][0] << " on position [" << i << "][" << j << "]" << std::endl;
                PossibleNumbers[i][j].clear();

                FilterWrittenNumbers();
                usedNumbers++;
                instantNumbers = true;
            }
            else if (!Sudoku[i][j] && PossibleNumbers[i][j].size() > 1)
            {
                // check for uniqueness in specific possible number
                for (unsigned checker = 0; checker < PossibleNumbers[i][j].size(); checker++)
                {
                    auto numberToCheck = PossibleNumbers[i][j][checker];
                    auto countOfNumber = 0;
                    unsigned columnCheck = j / ColumnsHorizontal;
                    unsigned rowCheck = i / ColumnsVertical;
                    for (unsigned rowToCheck = rowCheck * ColumnsVertical; countOfNumber < 2 && rowToCheck < ColumnsVertical * (rowCheck + 1); rowToCheck++)
                    {
                        for (unsigned columnToCheck = columnCheck * ColumnsHorizontal;countOfNumber < 2 && columnToCheck < ColumnsHorizontal * (columnCheck + 1); columnToCheck++)
                        {
                            if (Sudoku[rowToCheck][columnToCheck] == numberToCheck)
                                countOfNumber = 10;

                            auto tryToFind = std::find(PossibleNumbers[rowToCheck][columnToCheck].begin(), PossibleNumbers[rowToCheck][columnToCheck].end(), numberToCheck);
                            if (tryToFind != PossibleNumbers[rowToCheck][columnToCheck].end())
                                countOfNumber++;
                        }
                    }
                    // Specific is only if 1 is in whole 3x3 column
                    if (countOfNumber == 1)
                    {
#ifdef _DEBUGTEST
                        PrintSudoku();
#endif
                        Sudoku[i][j] = numberToCheck;
                        std::cout << "Writing number according to column uniqueness: " << numberToCheck<< " on position [" << i << "][" << j << "]" << std::endl;
                        PossibleNumbers[i][j].clear();
                        FilterWrittenNumbers();
                        usedNumbers++;
                        instantNumbers = true;
                        continue;
                    }

                    unsigned countOfNumberInRow = 0;
                    unsigned countOfNumberInColumn = 0;
                    for (unsigned field = 0; field < SudokuX; field ++)
                    {
                        auto foundNumColumn = std::find(PossibleNumbers[i][field].begin(), PossibleNumbers[i][field].end(), numberToCheck);
                        if (foundNumColumn != PossibleNumbers[i][field].end())
                        {
                            if (!Sudoku[i][field])
                                countOfNumberInColumn++;
                        }

                        auto foundNumRow = std::find(PossibleNumbers[field][j].begin(), PossibleNumbers[field][j].end(), numberToCheck);
                        if (foundNumRow != PossibleNumbers[field][j].end()) {
                            if (!Sudoku[field][j])
                                countOfNumberInRow++;
                        }
                    }

                    if (countOfNumberInRow == 1 || countOfNumberInColumn == 1)
                    {
#ifdef _DEBUGTEST
                        PrintSudoku();
#endif
                        if (Sudoku[i][j])
                            continue;
                        std::cout << "Writing number according to row uniqueness: " << numberToCheck << " on position [" << i << "][" << j << "]" << std::endl;
                        Sudoku[i][j] = numberToCheck;
                        PossibleNumbers[i][j].clear();
                        FilterWrittenNumbers();
                        usedNumbers++;
                        instantNumbers = true;
                        continue;
                    }
                }

            }
        }
    }
#ifdef _DEBUGTEST
    std::cout << "Sudoku solved: " << float(usedNumbers) * 100.0f / 81.0f << "%" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
#endif
    if (instantNumbers)
    {
       // PrintSudoku();
        FilterSudoku();
        return;
    }
    static bool finish = false;
    if (!finish)
    {
        finish = true;
        if ((usedNumbers != 81 || !CheckSudoku()))
        {
            PrintPossibleNumbers();
            std::cerr << "Sudoku not solved!!!" << std::endl;
        }
    }

}

bool CheckSudoku()
{
    for (int i = 0; i < SudokuX; i++)
    {
        for (int j = 0; j < SudokuY; j++)
        {
            if (Sudoku[i][j])
            {
                unsigned int row = i / ColumnsVertical;
                unsigned int column = j / ColumnsHorizontal;

                for (int k = 0; k < SudokuX; k++)
                {
                    if (Sudoku[i][k] && k != j)
                        if (Sudoku[i][k] == Sudoku[i][j])
                            return false;

                    if (Sudoku[k][j] && k != i)
                        if (Sudoku[k][j] == Sudoku[i][j])
                            return false;

                    unsigned int desiredRow = row * ColumnsVertical + k / ColumnsVertical;
                    unsigned int desiredColumn = column * ColumnsHorizontal + k % ColumnsHorizontal;
                    if (!(desiredRow == i && desiredColumn == j))
                    {
                        if (Sudoku[desiredRow][desiredColumn])
                            if (Sudoku[desiredRow][desiredColumn] == Sudoku[i][j])
                                return false;
                    }
                }
            }
        }
    }
    return true;
}

template<typename T>
void FilterTwoVectors(std::vector<T>& vec1, std::vector<T>& vec2, const int numberToCheck)
{

}

std::vector<unsigned> ColumnMatch(const unsigned _columnX, const unsigned _columnY)
{
    std::unordered_map<unsigned, unsigned> numberCounting;
    std::unordered_map<unsigned, std::vector<std::pair<unsigned , unsigned>>> elemWithColumn;
    unsigned numberOfColumnsToCount = 2;
    unsigned columnStartX = _columnX / ColumnsVertical;
    unsigned columnStartY = _columnY / ColumnsHorizontal;
    for (unsigned row = columnStartX * ColumnsHorizontal; row < columnStartX * ColumnsHorizontal + ColumnsHorizontal; row ++)
    {
        for (unsigned column = columnStartY * ColumnsVertical; column < columnStartY * ColumnsVertical + ColumnsVertical; column++)
        {
            for (const auto& num : PossibleNumbers[row][column])
            {
                numberCounting[num]++;
                if (!elemWithColumn.contains(num))
                {
                    elemWithColumn[num] = std::vector<std::pair<unsigned , unsigned>>();
                }
                elemWithColumn[num].push_back(std::pair(row, column));
            }
        }
    }

    for (auto it = numberCounting.begin(); it != numberCounting.end(); it++)
    {
        std::cout << "Number: " << it->first << " counting" << it->second << std::endl;
    }

    for (auto it : elemWithColumn)
    {
        std::cout << "Number: " << it.first <<  std::endl;
        for (auto it2 : it.second)
        {
            std::cout << "[" << it2.first << "," << it2.second << "]" << std::endl;
        }
    }



    return std::vector<unsigned >();
}

#define Recursive
#ifdef Recursive
void SolveSudoku(unsigned posX, unsigned posY)
{
    if (sudokuSolved)
    {
        return;
    }

    bool sudokuOk = CheckSudoku();
    if (sudokuOk)
    {
        if (posX == SudokuX - 1 && posY == SudokuY - 1)
        {
            sudokuSolved = true;
            return;
        }
    }
    else
    {
        return;
    }

    if (!CheckSudoku())
    {
        return;
    }
    for (unsigned i = posX; i < SudokuX; i++)
    {
        for (unsigned j = posY; j < SudokuY; j++)
        {
            if (!Sudoku[i][j])
            {
                unsigned int tries = PossibleNumbers[i][j].size();
                for (int k = 0; k < tries; k++)
                {
                    unsigned int possibleNumber = PossibleNumbers[i][j][k];
                    Sudoku[i][j] = possibleNumber;
                    SolveSudoku(i, j);
                    if (sudokuSolved)
                        return;
                    Sudoku[i][j] = 0;
                }
            }
        }
    }
}
#else
void SolveSudoku()
{

}
#endif

void PrintPossibleNumbers()
{
    for (unsigned i = 0; i < SudokuX; i++) {
        for (unsigned j = 0; j < SudokuY; j++)
        {
            std::cout << "[" << i << "]" << "[" << j << "]" <<std::endl;
            for (auto& value : PossibleNumbers[i][j])
            {
                std::cout << value << " ";
            }
            std::cout << std::endl;
        }
    }
}

#ifdef _DEBUGTEST
#undef _DEBUGTEST
#endif

int main() {
    LoadFile("sudoku.txt");
    FilterSudoku();
   // PrintSudoku();
//    PrintPossibleNumbers();
//    if (usedNumbers == SudokuX * SudokuY)
//        PrintSudoku();

    ColumnMatch(0,0);
}
