#include "GA.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <cmath>
#include <cassert>
#include <windows.h>
using namespace std;

namespace Random {
    mt19937 mt(random_device{}());

    template<typename T>
    T getRandom(T min, T max, typename enable_if<is_integral<T>::value >::type*) {
        return uniform_int_distribution<T>(min, max)(mt);
    }

    template<typename T>
    T getRandom(T min, T max, typename enable_if<is_floating_point<T>::value >::type*) {
        return uniform_real_distribution<T>(min, max)(mt);
    }

    template<typename Container, typename UnaryFunction>
    size_t rouletteWheel(double total, const Container& container, UnaryFunction&& func) {
        double target = getRandom(0.0, total);
        size_t index = 0;
        for (; index < container.size(); ++index) {
            target -= func(index);
            if (target <= 0.0) {
                break;
            }
        }
        return index;
    }

    template<typename Container>
    size_t rouletteWheel(double total, const Container& container) {
        auto func = [&container](size_t index) { return container[index]; };
        return rouletteWheel(total, container, func);
    }
}

GA::GA(const string& target)
    :mTarget(target), mTimer(clock()), generations(1){ //構造函數成員初始化列表
    initGeneration();
}

void GA::run() {
    while (mBest.dna != mTarget) {
        nextGeneration();
        showGeneration();
        generations++;
    }
    printf_s("Generations：%d\n", generations - 1);
}

void GA::initGeneration() {
    generate_n(back_inserter(mPopulation), MAX_POPULATION,
        [this]() -> DNA {
            DNA d;
            generate_n(back_inserter(d.dna), mTarget.size(), []() { return Random::getRandom(32, 126); });
            d.fitness = calcFitness(d.dna);
            mBest = getBest(mBest, d);
            return d;
        }
    );
}

void GA::nextGeneration() {
    DNA nextBest = mBest;
    vector<DNA> newGeneration;
    newGeneration.reserve(mPopulation.size());

    for (const auto& p : mPopulation) {
        if (Random::getRandom(0.0, 1.0) < CROSSOVER_RATE) {
            DNA child = crossOver();
            newGeneration.push_back(child);
        }
        else {
            newGeneration.push_back(p);
        }

        DNA& child = newGeneration.back();
        mutate(child);

        child.fitness = calcFitness(child.dna);

        nextBest = getBest(mBest, child);
    }

    newGeneration.pop_back();
    newGeneration.push_back(mBest);
    mBest = nextBest;
    mPopulation = newGeneration;
}

void GA::showGeneration() {
    printf_s("%dth：%s\n", generations, mBest.dna.c_str());
}

GA::DNA GA::crossOver() {
    double totalFitness = accumulate(mPopulation.begin(), mPopulation.end(), 0.0, [](double count, const DNA& d) { return count + d.fitness; });
    auto func = [this](size_t index) { return mPopulation[index].fitness; };

    const DNA& parent1 = mPopulation[Random::rouletteWheel(totalFitness, mPopulation, func)];
    const DNA& parent2 = mPopulation[Random::rouletteWheel(totalFitness, mPopulation, func)];

    DNA child;
    child.dna.reserve(parent1.dna.size());

    for (size_t i = 0; i < parent1.dna.size(); ++i) {
        child.dna.push_back(Random::getRandom(0, 1) == 0 ? parent1.dna[i] : parent2.dna[i]);
    }

    return child;
}

void GA::mutate(DNA& child) {
    for (auto& i : child.dna) {
        if (Random::getRandom(0.0, 1.0) < MUTATION_RATE) {
            i = Random::getRandom(32, 126);
        }
    }
}

double GA::calcFitness(const string& s) const {
    double r = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == mTarget[i]) {
            ++r;
        }
    }

    return pow(r / mTarget.size(), 4);
}

GA::DNA GA::getBest(const DNA& currentBest, const DNA& other) const {
    return currentBest.fitness > other.fitness ? currentBest : other;
}

string GA::best() const {
    return mBest.dna;
}

string entry(string targetStr) {
    if (targetStr.empty()) {
        MessageBox(NULL, L"targetStr cannot be empty!", L"錯誤", MB_OK);
        exit(0);
    }
    printf_s("Target String：%s\n", targetStr.c_str());
    GA ga(targetStr);
    ga.run();
    return ga.best();
}