#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <random>

using namespace std;

typedef vector<pair<double, int>> Chromosome;

struct Item {
    int weight, value;
};

int N, C;
vector<Item> items;
vector<pair<Chromosome, int>> population;

// Gerador de números aleatórios global
random_device rd;
mt19937 rng(rd()); // Inicializa com uma semente aleatória do hardware
uniform_real_distribution<double> dist(0.001, 0.999); // Intervalo ajustado

void generate_population(int pop_size) {
    population.clear();
    for (int i = 0; i < pop_size; ++i) {
        Chromosome chromosome;
        for (int j = 0; j < N; ++j) {
            double gene = round(dist(rng) * 1000) / 1000.0; // Gera um número entre 0.001 e 0.999
            chromosome.emplace_back(gene, j);
        }
        sort(chromosome.rbegin(), chromosome.rend());
        population.emplace_back(chromosome, 0);
    }
}

int decode_chromosome(const Chromosome &chromosome, vector<bool> &solution) {
    solution.assign(N, false);
    int total_weight = 0, total_value = 0;
    for (auto &[gene, idx] : chromosome) {
        if (total_weight + items[idx].weight <= C) {
            solution[idx] = true;
            total_weight += items[idx].weight;
            total_value += items[idx].value;
        }
    }
    return total_value;
}

void evaluate_population() {
    for (auto &individuo : population) {
        vector<bool> solution;
        individuo.second = decode_chromosome(individuo.first, solution);
    }
    sort(population.begin(), population.end(), [](const auto &a, const auto &b) {
        return a.second > b.second;
    });
}

Chromosome crossover(const Chromosome &elite, const Chromosome &normal) {
    Chromosome offspring = elite;
    uniform_real_distribution<double> crossover_prob(0.0, 1.0);
    for (size_t i = 0; i < elite.size(); ++i) {
        if (crossover_prob(rng) > 0.7) {
            offspring[i].first = normal[i].first;
        }
    }
    sort(offspring.rbegin(), offspring.rend());
    return offspring;
}

void evolve_population(int pop_size) {
    int elite_size = pop_size * 0.2;
    int new_random_size = pop_size * 0.1;
    int crossover_size = pop_size - elite_size - new_random_size;
    
    vector<pair<Chromosome, int>> new_population(population.begin(), population.begin() + elite_size);
    
    for (int i = 0; i < new_random_size; ++i) {
        Chromosome new_chromosome;
        for (int j = 0; j < N; ++j) {
            double gene = round(dist(rng) * 1000) / 1000.0; // Mantém três casas decimais
            new_chromosome.emplace_back(gene, j);
        }
        sort(new_chromosome.rbegin(), new_chromosome.rend());
        new_population.emplace_back(new_chromosome, 0);
    }
    
    for (int i = 0; i < crossover_size; ++i) {
        int elite_idx = uniform_int_distribution<int>(0, elite_size - 1)(rng);
        int normal_idx = uniform_int_distribution<int>(elite_size, pop_size - 1)(rng);
        Chromosome offspring = crossover(population[elite_idx].first, population[normal_idx].first);
        new_population.emplace_back(offspring, 0);
    }
    
    population = move(new_population);
    evaluate_population();
}

void read_input(const string &filename) {
    ifstream infile(filename);
    if (!infile) {
        cerr << "Erro ao abrir o arquivo!" << endl;
        exit(1);
    }
    infile >> N >> C;
    items.resize(N);
    for (int i = 0; i < N; ++i) {
        int id;
        infile >> id >> items[i].weight >> items[i].value;
    }
    infile.close();
}

void print_population(int generation) {
    cout << "\nGeração " << generation << ":\n";
    for (size_t i = 0; i < population.size(); ++i) {
        cout << "Cromossomo " << i + 1 << ": ";
        cout << "FITNESS: " << population[i].second << endl;
    }
    cout << endl;
}

int main() {
    read_input("input.txt");
    int population_size = 1000;
    generate_population(population_size);
    evaluate_population();
    print_population(0);
    for (int gen = 1; gen <= 1000; ++gen) {
        evolve_population(population_size);
        print_population(gen);
    }
    return 0;
}
