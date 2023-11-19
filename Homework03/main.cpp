#include <iostream>
#include <vector>
#include <unordered_set>
#include <random>
#include <algorithm>

int M, N;
const double CROSSOVER_RATE = 0.53;
const double MUTATION_RATE = 0.013;
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
    std::random_device rd;
    std::mt19937 gen(rd());
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

std::vector<Individual> tournamentSelection(const std::vector<Individual>& population, int tournamentSize) {
    std::vector<Individual> selectedParents;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, population.size() - 1);

    while (selectedParents.size() < 2) {
        // Randomly select 'tournamentSize' individuals for the tournament
        std::vector<Individual> tournamentParticipants;
        for (int i = 0; i < tournamentSize; ++i) {
            int index = dis(gen);
            tournamentParticipants.push_back(population[index]);
        }

        // Find the best individual within the tournament
        Individual best = tournamentParticipants[0];
        for (size_t i = 1; i < tournamentParticipants.size(); ++i) {
            if (tournamentParticipants[i].fitness() > best.fitness()) {
                best = tournamentParticipants[i];
            }
        }

        selectedParents.push_back(best);
    }

    return selectedParents;
}

std::vector<Individual> crossover(const std::vector<Individual>& parents)
{
    std::vector<Individual> children;

    std::vector<bool> child1(parents[0].bits.begin(), parents[0].bits.begin() + N / 2);
    child1.insert(child1.end(), parents[1].bits.begin() + N / 2, parents[1].bits.end());

    std::vector<bool> child2(parents[1].bits.begin() + N / 2, parents[1].bits.end());
    child2.insert(child2.end(), parents[0].bits.begin(), parents[0].bits.begin() + N / 2);

    children.push_back(Individual{child1});
    children.push_back(Individual{child2});

    return children;
}

void mutate(std::vector<Individual>& individuals)
{
    std::random_device rd;
    std::mt19937 gen(rd());
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

std::vector<Individual> nextGeneration(const std::vector<Individual>& population)
{
    std::vector<Individual> nextGeneration;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    while (nextGeneration.size() < population.size())
    {
        std::vector<Individual> children;

        std::vector<Individual> parents = tournamentSelection(population, 2);

        if (dis(gen) < REPRODUCTION_RATE)
        {
            children = parents;
        }
        else
        {
            if (dis(gen) < CROSSOVER_RATE)
            {
                children = crossover(parents);
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

void printGeneration(const std::vector<Individual>& population)
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
    std::cout << "Average fitness: " << averageFitness << std::endl;
    std::cout << "Max fitness: " << maxFitness << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;
}

void solveKnapsack()
{
    std::vector<Individual> population = generateInitialPopulation();

    for (int i = 0; i < 10; ++i)
    {
        printGeneration(population);
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

