#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <utility>

class BlocksState
{
private:
    std::vector<std::vector<int>> blocks;
    int manhattanDistance;
    std::string road;

public:
    BlocksState(const std::vector<std::vector<int>>& blocks, int manhattanDistance, const std::string& road)
        : blocks(blocks), manhattanDistance(manhattanDistance), road(road) {}

    bool operator == (const BlocksState& other) const
    {
        return this->blocks == other.blocks;
    }

    int getManhattanDistance() const
    {
        return this->manhattanDistance;
    }

    const std::vector<std::vector<int>>& getBlocks() const 
    {
        return this->blocks;
    }

    const std::string& getRoad() const
    {
        return this->road;
    }
};

struct Node
    {
        BlocksState* value;
        Node* next;

        Node(BlocksState* value, Node* next = nullptr)
            : value(value), next(next) {}
    };

class PriorityStack
{
private:
    Node* head;

public:
    PriorityStack(BlocksState* initialState)
        : head(new Node(initialState)) {}

    Node* pop()
    {
        if (!this->head)
        {
            return nullptr;
        }

        Node* result = this->head;
        this->head = this->head->next;
    
        return result;
    }

    std::size_t size() const
    {
        Node* temp = this->head;
        std::size_t result = 0;

        while (temp)
        {
            ++result;
            temp = temp->next;
        }

        return result;
    }

    void push(BlocksState* newState)
    {
        Node* newNode = new Node(newState);

        if (!this->head)
        {
            this->head = newNode;
            return;
        }

        if (this->head->value->getManhattanDistance() > newNode->value->getManhattanDistance())
        {
            newNode->next = this->head;
            this->head = newNode;
        }
        else
        {
            Node* prev = this->head;
            Node* current = this->head->next;

            while (true)
            {
                if (!current)
                {
                    prev->next = newNode;
                    break;
                }

                if (current->value->getManhattanDistance() > newNode->value->getManhattanDistance())
                {
                    prev->next = newNode;
                    newNode->next = current;
                    break;
                }

                current = current->next;
                prev = prev->next;
            }
        }
    }
};

int getManhattanDistance(const std::vector<std::vector<int>>& blocks)
{
    int result = 0;

    for (int i = 0; i < blocks.size(); ++i)
    {
        for (int j = 0; j < blocks[0].size(); ++j)
        {
            int blockValue = blocks[i][j];

            if (blockValue == 0)
            {
                continue;
            }

            int targetX = (blockValue - 1) / blocks[0].size();
            int targetY = (blockValue - 1) % blocks[0].size();

            result += (std::abs(i - targetX) + std::abs(j - targetY));
        }
    }

    return result;
}

std::pair<int, int> getZeroCoordinates(const std::vector<std::vector<int>> blocks)
{
    for (int i = 0; i < blocks.size(); ++i)
    {
        for (int j = 0; j < blocks[0].size(); ++j)
        {
            if (blocks[i][j] == 0)
            {
                return std::make_pair(i, j);
            }
        }
    }

    return std::make_pair(-1, -1);
}

BlocksState* createBlocksState(const std::vector<std::vector<int>>& blocks, int zeroX, int zeroY, int newZeroX, int newZeroY, const std::string& road)
{
    if (newZeroX < 0 || newZeroX >= blocks.size() || newZeroY < 0 || newZeroY >= blocks[0].size())
    {
        return nullptr;
    }

    std::vector<std::vector<int>> newBlocks = blocks;
    newBlocks[zeroX][zeroY] = newBlocks[newZeroX][newZeroY];
    newBlocks[newZeroX][newZeroY] = 0;

    return new BlocksState(newBlocks, getManhattanDistance(newBlocks), road);
}

bool contains(const std::vector<BlocksState*>& states, BlocksState* state)
{
    for (auto current : states)
    {
        if (current->getBlocks() == state->getBlocks())
        {
            return true;
        }
    }

    return false;
}

void print (const std::vector<std::vector<int>>& blocks)
{
    for (std::size_t i = 0; i < blocks.size(); ++i)
    {
        for (std::size_t j = 0; j < blocks[0].size(); ++j)
        {
            std::cout << blocks[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "-----------" << std::endl;
}

BlocksState* solve(PriorityStack& stack, std::vector<BlocksState*>& visited)
{
    Node* currentState = stack.pop();

    while (currentState)
    {
        std::pair<int, int> zeroCoordinates = getZeroCoordinates(currentState->value->getBlocks());
        int zeroX = zeroCoordinates.first;
        int zeroY = zeroCoordinates.second;

        std::vector<std::pair<int, int>> moves;
        moves.push_back(std::make_pair(zeroX, zeroY + 1));
        moves.push_back(std::make_pair(zeroX, zeroY - 1));
        moves.push_back(std::make_pair(zeroX + 1, zeroY));
        moves.push_back(std::make_pair(zeroX - 1, zeroY));

        int direction = 0;

        for (auto move : moves)
        {
            ++direction;
            std::string road = currentState->value->getRoad() + std::to_string(direction);

            BlocksState* newBlocksState = createBlocksState(currentState->value->getBlocks(), zeroX, zeroY, move.first, move.second, road);

            if (newBlocksState)
            {
                if (newBlocksState->getManhattanDistance() == 0)
                {
                    return newBlocksState;
                }

                if (!contains(visited, newBlocksState))
                {
                    stack.push(newBlocksState);
                    visited.push_back(newBlocksState);
                }
            }
        }

        currentState = stack.pop();
    }

    return nullptr;
}

bool isSolvable(const std::vector<std::vector<int>>& blocks) 
{
    std::vector<int> flatBlocks;
    for (const auto& row : blocks) 
    {
        flatBlocks.insert(flatBlocks.end(), row.begin(), row.end());
    }
    
    int inversions = 0;
    for (int i = 0; i < flatBlocks.size(); ++i) 
    {
        for (int j = i + 1; j < flatBlocks.size(); ++j) 
        {
            if (flatBlocks[i] > 0 && flatBlocks[j] > 0 && flatBlocks[i] > flatBlocks[j]) 
            {
                inversions++;
            }
        }
    }
    
    std::pair<int, int> zeroCoordinates = getZeroCoordinates(blocks);
    int zeroRow = zeroCoordinates.first + 1; // Add 1 because rows are 1-based
    
    // Check if the puzzle is solvable
    return (inversions % 2 == 0 && zeroRow % 2 == 1) || (inversions % 2 == 1 && zeroRow % 2 == 0);
}


int main ()
{
    std::size_t n;
    std::cin >> n;
    std::vector<std::vector<int>> blocks(std::sqrt(n + 1), std::vector<int>(std::sqrt(n + 1)));

    // int i;
    // std::cin >> i;

    for (std::size_t k = 0; k * k < n + 1; ++k)
    {
        for (std::size_t j = 0; j * j < n + 1; ++j)
        {
            std::cin >> blocks[k][j];
        }
    }

    if (!isSolvable(blocks))
    {
        std::cout << "No solutions" << std::endl;
        return 1;
    }

    BlocksState* initialState = new BlocksState(blocks, getManhattanDistance(blocks), "");

    if (initialState->getManhattanDistance() == 0)
    {
        std::cout << "The input is the solution" << std::endl;
    }
    else
    {
        PriorityStack stack(initialState);
        std::vector<BlocksState*> visited;

        BlocksState* solution  = solve(stack, visited);

        if (solution)
        {
            std::cout << solution->getRoad().length() << std::endl;

            for (char ch : solution->getRoad())
            {
                switch (ch)
                {
                case '1':
                    std::cout << "left" << std::endl;
                    break;
                case '2':
                    std::cout << "right" << std::endl;
                    break;
                case '3':
                    std::cout << "up" << std::endl;
                    break;
                case '4':
                    std::cout << "down" << std::endl;
                    break;
                default:
                    break;
                }
            }
        }
        else
        {
            std::cout << "No solutions" << std::endl;
        }
    }
}