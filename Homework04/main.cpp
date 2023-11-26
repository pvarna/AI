#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

const char EMPTY = '-';
const char PLAYER_X = 'X';
const char PLAYER_O = 'O';

const char TIE = 'T';

void printBoard(const std::vector<char>& board) 
{
    std::cout << "-------------" << std::endl;
    for (int i = 0; i < 3; ++i) 
    {
        std::cout << "| " << board[i * 3] << " | " << board[i * 3 + 1] << " | " << board[i * 3 + 2] << " |" << std::endl;
        std::cout << "-------------" << std::endl;
    }
}

char checkWinner(const std::vector<char>& board) 
{
    std::vector<std::vector<int>> winningCombinations = 
    {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},  // rows
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},  // columns
        {0, 4, 8}, {2, 4, 6}  // diagonals
    };

    for (const auto& combination : winningCombinations) 
    {
        if (board[combination[0]] == board[combination[1]] && board[combination[1]] == board[combination[2]] && board[combination[0]] != EMPTY) 
        {
            return board[combination[0]];
        }
    }

    if (std::find(board.begin(), board.end(), EMPTY) == board.end()) 
    {
        return TIE;
    }

    return EMPTY;
}

int evaluate(const std::vector<char>& board) 
{
    char winner = checkWinner(board);

    if (winner == PLAYER_X) 
    {
        return 1;
    } 
    else if (winner == PLAYER_O) 
    {
        return -1;
    } 
    else {
        return 0;
    }
}

int maxMove(std::vector<char>& board, int alpha, int beta);
int minMove(std::vector<char>& board, int alpha, int beta);

int maxMove(std::vector<char>& board, int alpha, int beta) 
{
    char winner = checkWinner(board);
    if (winner != EMPTY) 
    {
        return evaluate(board);
    }

    int maxEval = INT_MIN;
    for (int i = 0; i < 9; ++i) 
    {
        if (board[i] == EMPTY) 
        {
            board[i] = PLAYER_X;
            int evalScore = minMove(board, alpha, beta);
            board[i] = EMPTY;
            maxEval = std::max(maxEval, evalScore);
            alpha = std::max(alpha, evalScore);
            if (beta <= alpha) 
            {
                break;
            }
        }
    }
    
    return maxEval;
}

int minMove(std::vector<char>& board, int alpha, int beta) 
{
    char winner = checkWinner(board);
    if (winner != EMPTY) 
    {
        return evaluate(board);
    }

    int minEval = INT_MAX;
    for (int i = 0; i < 9; ++i) 
    {
        if (board[i] == EMPTY) 
        {
            board[i] = PLAYER_O;
            int evalScore = maxMove(board, alpha, beta);
            board[i] = EMPTY;
            minEval = std::min(minEval, evalScore);
            beta = std::min(beta, evalScore);
            if (beta <= alpha) 
            {
                break;
            }
        }
    }

    return minEval;
}

int findBestMove(std::vector<char>& board) 
{
    int bestScore = INT_MIN;
    int bestMove = -1;

    for (int i = 0; i < 9; ++i) 
    {
        if (board[i] == EMPTY) 
        {
            board[i] = PLAYER_X;
            int moveScore = minMove(board, INT_MIN, INT_MAX);
            board[i] = EMPTY;

            if (moveScore > bestScore) 
            {
                bestScore = moveScore;
                bestMove = i;
            }
        }
    }

    return bestMove;
}

int main() 
{
    std::vector<char> board(9, EMPTY);

    std::cout << "Enter 0 if the AI will be first and 1 if you will be first: ";
    bool isAiSecond;
    std::cin >> isAiSecond;

    while (true) 
    {
        printBoard(board);
        char winner = checkWinner(board);

        if (winner != EMPTY) 
        {
            if (winner == 'T') 
            {
                std::cout << "It's a tie!" << std::endl;
            } 
            else 
            {
                std::cout << "Player " << winner << " wins!" << std::endl;
            }
            break;
        }

        if (std::count(board.begin(), board.end(), EMPTY) % 2 != !isAiSecond) 
        {
            int move;
            while (true) 
            {
                int x, y;
                std::cout << "Enter O's move (two numbers in the interval [1;3]): ";
                std::cin >> x >> y;
                move = (x - 1) * 3 + (y - 1);
                if (board[move] == EMPTY) 
                {
                    board[move] = PLAYER_O;
                    break;
                } 
                else 
                {
                    std::cout << "Invalid move! Try again." << std::endl;
                }
            }
        } 
        else 
        {
            int aiMove = findBestMove(board);
            board[aiMove] = PLAYER_X;
        }
    }

    return 0;
}