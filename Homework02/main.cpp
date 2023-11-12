#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <utility>
#include <random>

int swaps = 0;

void print(const std::vector<int>& nQueens)
{
    int n = nQueens.size();
    for (int row = 0; row < n; ++row) 
    {
        for (int col = 0; col < n; ++col) 
        {
            if (nQueens[col] == row) 
            {
                std::cout << " * ";
            } 
            else 
            {
                std::cout << " _ ";
            }
        }
        std::cout << std::endl << std::endl; 
    }
}

std::vector<int> getKnightMoves(int row, int n) 
{
    std::vector<int> knightMoves = {2, -1};
    std::vector<int> moves;
    for (int move : knightMoves) 
    {
        int sum = move + row;
        sum = (sum < 0) ? sum + n : sum;
        moves.push_back(sum % n);
    }
    return moves;
}

void initilise(std::vector<int>& nQueens)
{
    int n = nQueens.size();
    nQueens[0] = 0;

    // std::cout << "New initialisation" << std::endl;
    
    for (int col = 1; col < n; ++col)
    {
        int prevQueenRow = nQueens[col - 1];
        std::vector<int> knightRows = getKnightMoves(prevQueenRow, n);
        std::vector<int> possibleRows;
        for (int row : knightRows)
        {
            bool rowNotInBoard = true;

            for (int idx = 0; idx < col; ++idx)
            {
                if (row == nQueens[idx])
                {
                    rowNotInBoard = false;
                    break;
                }
            }

            if (rowNotInBoard)
            {
                possibleRows.push_back(row);
            }
        }
        nQueens[col] = possibleRows[0];
    }
}

std::pair<int, int> mostConflictedQueen(int n,
                                        const std::vector<int>& nQueens, 
                                        const std::vector<int>& queensPerRow,
                                        const std::vector<int>& queensPerD1,
                                        const std::vector<int>& queensPerD2
                                        )
{
    int maxConflicts = 0;
    std::vector<int> possibleCols;

    for (int col = 0; col < n; ++col)
    {
        int row = nQueens[col];

        int conflicts = queensPerD1[row - col + n - 1] + queensPerD2[row + col] + queensPerRow[row] - 3;
        if (conflicts > maxConflicts)
        {
            maxConflicts = conflicts;
            possibleCols.clear();
            possibleCols.push_back(col);
        }
        else if (conflicts == maxConflicts)
        {
            possibleCols.push_back(col);
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> distribution(0, possibleCols.size() - 1);
    int randomIndex = distribution(gen);
    int randomElement = possibleCols[randomIndex];

    return std::make_pair(randomElement, maxConflicts);
}

void moveQueen(int n,
               int col,
               std::vector<int>& nQueens, 
               std::vector<int>& queensPerRow,
               std::vector<int>& queensPerD1,
               std::vector<int>& queensPerD2)
{
    int minConflicts = n;
    int minConflictsRow = 0;

    for (int row = 0; row < n; ++row)
    {
        int conflicts = queensPerD1[row - col + n - 1] + queensPerD2[row + col] + queensPerRow[row];

        if (conflicts < minConflicts)
        {
            minConflicts = conflicts;
            minConflictsRow = row;
        }
    }

    int row = nQueens[col];

    --queensPerD1[row - col + n - 1];
    --queensPerD2[row + col];
    --queensPerRow[row];
    ++queensPerD1[minConflictsRow - col + n - 1];
    ++queensPerD2[minConflictsRow + col];
    ++queensPerRow[minConflictsRow];

    nQueens[col] = minConflictsRow;
}

std::vector<int> solve(int n, int maxIterations)
{
    std::vector<int> nQueens(n, -1);
    initilise(nQueens);

    std::vector<int> queensPerRow(n, 0);
    std::vector<int> queensPerD1(2 * n - 1, 0);
    std::vector<int> queensPerD2(2 * n - 1, 0);

    for (int col = 0; col < n; ++col)
    {
        int row = nQueens[col];

        ++queensPerRow[row];
        ++queensPerD1[row - col + n - 1];
        ++queensPerD2[row + col];
    }

    for (int i = 0; i < maxIterations; ++i)
    {
        std::pair<int, int> pair = mostConflictedQueen(n, nQueens, queensPerRow, queensPerD1, queensPerD2);
        int col = pair.first;
        int conflicts = pair.second;

        if (conflicts == 0)
        {
            return nQueens;
        }

        ++swaps;

        moveQueen(n, col, nQueens, queensPerRow, queensPerD1, queensPerD2);
    }

    return solve(n, maxIterations);
}

int main ()
{
    int n;
    std::cin >> n;

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> nQueens = solve(n, 100);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    if (n <= 100)
    {
        print(nQueens);
    }
    else
    {
        std::cout << "Swaps: " << swaps << std::endl;
        std::cout << "Duration: " << duration.count() / 1000.0 << " s." << std::endl;
    }

    return 0;
}