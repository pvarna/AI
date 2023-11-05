#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <utility>
#include <unordered_map>
#include <queue>
#include <chrono>

std::unordered_map<int, int> correctIndeces;
class BlocksState
{
private:
    std::vector<std::vector<int>> blocks;
    int manhattanDistance;
    std::string road;
    std::size_t level;

public:
    BlocksState(const std::vector<std::vector<int>>& blocks, int manhattanDistance, const std::string& road, std::size_t level)
        : blocks(blocks), manhattanDistance(manhattanDistance), road(road), level(level) {}

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

    const std::size_t getLevel() const
    {
        return this->level;
    }
};

struct Node
{
    BlocksState* value;
    Node* next;

    Node(BlocksState* value, Node* next = nullptr)
        : value(value), next(next) {}
};

struct CompareBlocksState
{
    bool operator()(const Node* lhs, const Node* rhs) const
    {
        return lhs->value->getManhattanDistance() + lhs->value->getLevel() > rhs->value->getManhattanDistance() + rhs->value->getLevel();
    }
};

class PriorityQueue
{
private:
    std::priority_queue<Node*, std::vector<Node*>, CompareBlocksState> queue;

public:
    PriorityQueue(BlocksState* initialState)
    {
        queue.push(new Node(initialState));
    }

    Node* pop()
    {
        if (queue.empty())
        {
            return nullptr;
        }

        Node* result = queue.top();
        queue.pop();

        return result;
    }

    std::size_t size() const
    {
        return queue.size();
    }

    void push(BlocksState* newState)
    {
        queue.push(new Node(newState));
    }

    ~PriorityQueue()
    {
        while (!queue.empty())
        {
            Node* top = queue.top();
            queue.pop();
            delete top;  
        }
    }
};

int getManhattanDistance(const std::vector<std::vector<int>>& blocks)
{
    int result = 0;

    for (int i = 0; i < blocks.size(); ++i)
    {
        for (int j = 0; j < blocks.size(); ++j)
        {
            int blockValue = blocks[i][j];

            if (blockValue == 0)
            {
                continue;
            }

            int targetX = correctIndeces[blockValue] / blocks.size();
            int targetY = correctIndeces[blockValue] % blocks.size();

            result += (std::abs(i - targetX) + std::abs(j - targetY));
        }
    }

    return result;
}

std::pair<int, int> getZeroCoordinates(const std::vector<std::vector<int>> blocks)
{
    for (std::size_t i = 0; i < blocks.size(); ++i)
    {
        for (std::size_t j = 0; j < blocks[0].size(); ++j)
        {
            if (blocks[i][j] == 0)
            {
                return std::make_pair(i, j);
            }
        }
    }

    return std::make_pair(-1, -1);
}

BlocksState* createBlocksState(const std::vector<std::vector<int>>& blocks, int zeroX, int zeroY, int newZeroX, int newZeroY, const std::string& road, std::size_t level)
{
    if (newZeroX < 0 || newZeroX >= blocks.size() || newZeroY < 0 || newZeroY >= blocks[0].size())
    {
        return nullptr;
    }

    std::vector<std::vector<int>> newBlocks = blocks;
    newBlocks[zeroX][zeroY] = newBlocks[newZeroX][newZeroY];
    newBlocks[newZeroX][newZeroY] = 0;

    return new BlocksState(newBlocks, getManhattanDistance(newBlocks), road, level + 1);
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

int getInversionsCount(const std::vector<std::vector<int>>& blocks)
{
    std::vector<int> arr;

    for (auto row : blocks)
    {
        for (auto element : row)
        {
            arr.push_back(element);
        }
    }

    int inversionsCount = 0;
    for (std::size_t i = 0; i < arr.size() - 1; ++i)
    {
        for (std::size_t j = i + 1; j < arr.size(); ++j)
        {
            if (arr[j] && arr[i] &&  arr[i] > arr[j])
            {
                ++inversionsCount;
            }
        }
    }
    return inversionsCount;
}

bool isSolvable(const std::vector<std::vector<int>>& blocks, int zeroRowIndex)
{
    int inversionsCount = getInversionsCount(blocks);
 
    if (blocks.size() % 2 == 1)
    {
        return (inversionsCount % 2 == 0);
    }

    return ((inversionsCount + zeroRowIndex) % 2 == 1);
}


BlocksState* solve(PriorityQueue& queue, std::vector<BlocksState*>& visited)
{
    Node* currentState = queue.pop();

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

            BlocksState* newBlocksState = createBlocksState(currentState->value->getBlocks(), zeroX, zeroY, move.first, move.second, road, currentState->value->getLevel());

            if (newBlocksState)
            {
                if (newBlocksState->getManhattanDistance() == 0)
                {
                    return newBlocksState;
                }

                if (!contains(visited, newBlocksState))
                {
                    queue.push(newBlocksState);
                    visited.push_back(newBlocksState);
                }
                else
                {
                    delete newBlocksState;
                }
            }
        }

        delete currentState;
        currentState = queue.pop();
    }

    return nullptr;
}

int main ()
{
    std::size_t n;
    std::cin >> n;
    std::size_t side = std::sqrt(n + 1);
    std::vector<std::vector<int>> blocks(side, std::vector<int>(side));

    int input_i;
    std::cin >> input_i;

    auto start = std::chrono::high_resolution_clock::now();

    input_i = (input_i == -1) ? n : input_i;

    for (std::size_t i = 0; i < input_i; ++i)
    {
        correctIndeces.insert(std::make_pair(i + 1, i));
    }
    correctIndeces.insert(std::make_pair(0, input_i));
    for (std::size_t i = input_i + 1; i <= n; ++i)
    {
        correctIndeces.insert(std::make_pair(i, i));
    }

    int zeroRowIndex = 0;

    for (std::size_t k = 0; k < side; ++k)
    {
        for (std::size_t j = 0; j < side; ++j)
        {
            std::cin >> blocks[k][j];
            if (blocks[k][j] == 0)
            {
                zeroRowIndex = k;
            }
        }
    }

    BlocksState* initialState = new BlocksState(blocks, getManhattanDistance(blocks), "", 0);

    if (!isSolvable(initialState->getBlocks(), zeroRowIndex))
    {
        std::cout << -1 << std::endl;
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Program executed for " << duration.count() << " milliseconds (" << duration.count() / 1000.0 <<  " seconds)" << std::endl;
    }
    else if (initialState->getManhattanDistance() == 0)
    {
        std::cout << 0 << std::endl;
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Program executed for " << duration.count() << " milliseconds (" << duration.count() / 1000.0 <<  " seconds)" << std::endl;
    }
    else
    {
        PriorityQueue queue(initialState);
        std::vector<BlocksState*> visited;

        BlocksState* solution  = solve(queue, visited);

        if (solution)
        {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "Program executed for " << duration.count() << " milliseconds (" << duration.count() / 1000.0 <<  " seconds)" << std::endl;
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

            delete solution;
        }
        else
        {
            std::cout << -1 << std::endl;
        }

        for (BlocksState* state : visited)
        {
            delete state;
        }
    }

    delete initialState;

    return 0;
}