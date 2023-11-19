#include <iostream>
#include <vector>
#include <unordered_set>
#include <random>
#include <algorithm>

std::random_device rd;
std::mt19937 gen(rd());
int M, N;
const double CROSSOVER_RATE = 0.53;
const double MUTATION_RATE = 0.1;
const double REPRODUCTION_RATE = 0.15;

struct Item
{
    int weight;
    int value;
};

std::vector<Item> items;

struct Individual
{
    std::vector<bool> bits;

    struct HashFunction 
    {
        size_t operator()(const Individual& obj) const 
        {
            size_t hash = 0;
            // Combine hash values of individual bools in the vector
            for (const bool bit : obj.bits) 
            {
                // Update the hash value using bitwise operations
                hash ^= std::hash<bool>{}(bit) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            return hash;
        }
    };

    bool operator==(const Individual& other) const 
    {
        return bits == other.bits;
    }

    bool operator<(const Individual& other) const 
    {
        return this->fitness() < other.fitness();
    }

    int fitness() const
    {
        int sumValue = 0;
        int sumWeight = 0;

        for (int i = 0; i < N; ++i)
        {
            sumValue += (items[i].value * bits[i]);
            sumWeight += (items[i].weight * bits[i]);
        }

        return (sumWeight <= M) ? sumValue : 0;
    }

    std::string toString() const
    {
        std::string result = "";
        
        for (auto el : this->bits)
        {
            result += (el ? '1' : '0');
        }
        
        return result;
    }
};

std::vector<Individual> generateInitialPopulation() 
{
    std::unordered_set<Individual, Individual::HashFunction> population;
    std::uniform_int_distribution<int> dis(0, 1);

    while (population.size() < N) 
    {
        std::vector<bool> bits;
        
        for (int i = 0; i < N; ++i)
        {
            bits.push_back(dis(gen));
        }

        Individual temp = Individual{bits};
        population.insert(temp);
    }

    return std::vector<Individual>(population.begin(), population.end());
}

std::vector<Individual> selection(const std::vector<Individual>& population, int tournamentSize = 3) 
{
    std::vector<Individual> parents;

    while (parents.size() < 2) 
    {
        std::vector<Individual> tournament;

        // Randomly select individuals for the tournament
        while (tournament.size() < tournamentSize) 
        {
            int index = rand() % population.size();
            tournament.push_back(population[index]);
        }

        // Find the best individual in the tournament
        Individual best = tournament[0];
        for (int i = 1; i < tournament.size(); ++i) 
        {
            if (tournament[i].fitness() > best.fitness()) 
            {
                best = tournament[i];
            }
        }

        parents.push_back(best);
    }

    return parents;
}

std::vector<Individual> twoPointerCrossover(const std::vector<Individual>& parents)
{
    std::vector<Individual> children;

    std::uniform_int_distribution<int> dis(0, N - 1);
    int point1 = dis(gen);
    int point2 = dis(gen);

    // Ensure point1 and point2 are different
    while (point1 == point2) 
    {
        point2 = dis(gen);
    }

    // Make sure point1 is smaller than point2
    if (point1 > point2) 
    {
        std::swap(point1, point2);
    }

    std::vector<bool> child1, child2;

    // Perform crossover for each parent
    for (int i = 0; i < N; ++i)
    {
        if (i >= point1 && i <= point2) 
        {
            child1.push_back(parents[1].bits[i]);
            child2.push_back(parents[0].bits[i]);
        } 
        else 
        {
            child1.push_back(parents[0].bits[i]);
            child2.push_back(parents[1].bits[i]);
        }
    }

    children.push_back(Individual{child1});
    children.push_back(Individual{child2});

    return children;
}

std::vector<Individual> uniformCrossover(const std::vector<Individual>& parents)
{
    std::vector<Individual> children;
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    std::vector<bool> child1, child2;

    for (int i = 0; i < N; ++i) 
    {
        if (dis(gen) < 0.5) 
        { 
            child1.push_back(parents[0].bits[i]);
            child2.push_back(parents[1].bits[i]);
        } 
        else 
        {
            child1.push_back(parents[1].bits[i]);
            child2.push_back(parents[0].bits[i]);
        }
    }

    children.push_back(Individual{child1});
    children.push_back(Individual{child2});

    return children;
}


void mutate(std::vector<Individual>& individuals)
{
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    for (Individual& individual : individuals)
    {
        for (int i = 0; i < N; ++i)
        {
            if (dis(gen) < MUTATION_RATE)
            {
                individual.bits[i] = !individual.bits[i];
            }
        }
    }
}

bool compareIndividuals(const Individual& i1, const Individual& i2)
{
    return (i1.fitness() > i2.fitness());
}

std::vector<Individual> nextGeneration(const std::vector<Individual>& population)
{
    std::vector<Individual> nextGeneration;
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    // int numberOfElites = 2;

    // std::vector<Individual> sortedPopulation = population;
    // std::sort(sortedPopulation.begin(), sortedPopulation.end(), compareIndividuals);

    // nextGeneration.insert(nextGeneration.end(), sortedPopulation.begin(), sortedPopulation.begin() + numberOfElites);

    while (nextGeneration.size() < population.size())
    {
        std::vector<Individual> children;

        std::vector<Individual> parents = selection(population);

        if (dis(gen) < REPRODUCTION_RATE)
        {
            children = parents;
        }
        else
        {
            if (dis(gen) < CROSSOVER_RATE)
            {
                children = uniformCrossover(parents);
            }

            if (dis(gen) < MUTATION_RATE)
            {
                mutate(children);
            }
        }

        
        nextGeneration.insert(nextGeneration.end(), children.begin(), children.end());
    }

    return std::vector<Individual>(nextGeneration.begin(), nextGeneration.begin() + population.size());
}

void printGeneration(const std::vector<Individual>& population, int index)
{
    double sumFitness = 0.0;
    int maxFitness = 0;

    for (auto individual : population)
    {
        int fitness = individual.fitness();
        //std::cout << individual.toString() << " " << fitness << std::endl;
        sumFitness += fitness;
        maxFitness = std::max(fitness, maxFitness);
    }

    double averageFitness = sumFitness / population.size();
    // std::cout << "Average fitness: " << averageFitness << std::endl;
    //std::cout << "Max fitness: " << maxFitness << std::endl;
    if (index % 1000 == 0)
    {
        std::cout << maxFitness << std::endl;
    }
    //std::cout << "-----------------------------------------------------" << std::endl;
}

void solveKnapsack()
{
    std::vector<Individual> population = generateInitialPopulation();

    for (int i = 0; i < 10000; ++i)
    {
        printGeneration(population, i);
        population = nextGeneration(population);
    }
}

int main ()
{
    std::cin >> M >> N;

    for (int i = 0; i < N; ++i)
    {
        int weight, value;
        std::cin >> weight >> value;

        items.push_back(Item{weight, value});
    }

    solveKnapsack();

    return 0;
}