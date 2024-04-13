#include <iostream>
#include <array>
#include <set>
#include <thread>
#include <chrono>
//1. Самое простое решение задание, инвертирует все биты
void InvertBitInPointer(void * pointer, int length) {
    unsigned char* copy = (unsigned char*)pointer;
    for (int i = 0; i < length; i++) {
        try {
            copy[i] = ~copy[i];
        }
        catch(...){
            throw std::exception("Array out of bounds"); //Я считаю это лишним, но всякое может случиться...
        }
    }
}

//2. Пробуем "усложнить", теперь биты инвертируется в какой-то области
void InvertBitInPointer(void* pointer, int length, int start_bit, int end_bit) {
    unsigned char* copy = (unsigned char*)pointer;

    //высчитываем маску
    unsigned char mask = 0;
    for (int j = start_bit; j <= end_bit; j++) {
        mask |= 1 << j;
    }

    for (int i = 0; i < length; i++) {
        try {
            copy[i] ^= mask;
        }
        catch (...) {
            throw std::exception("Array out of bounds");
        }
    }
}

//3. Ещё "усложняем", теперь передаётся множество ивертируемых битов (по номеру)
void InvertBitInPointer(void* pointer, int length, const std::set<int>& bits) {
    unsigned char* copy = (unsigned char*)pointer;

    unsigned char mask = 0;
    for (auto& part : bits) {
        mask |= 1 << part;
    }

    for (int i = 0; i < length; i++) {
        try {
            copy[i] ^= mask;
        }
        catch (...) {
            throw "Array out of bounds";
        }
    }
}

//4. А теперь применяем знания по распаралелливанию, модифицируя 3. (Медленнее)
void ParallelInvertBitInPointer(void* pointer, int length, const std::set<int>& bits) {
    unsigned char* copy = (unsigned char*)pointer;

    unsigned char mask = 0;
    for (auto& part : bits) {
        mask |= 1 << part;
    }

    auto lambda_func = [](unsigned char* pointer, int length, unsigned char mask, int start) {
        for (int i = start; i < length; i += 2) {
            try {
                pointer[i] ^= mask;
            }
            catch (...) {
                throw "Array out of bounds";
            }
        }
        };

    //делаем асинхронно
    std::thread even(lambda_func, copy, length, mask, 0),
                odd(lambda_func, copy, length, mask, 1);
    even.join();
    odd.join();
}

int main()
{
    int* pointer = new int[5];
    pointer[0] = 17;
    pointer[1] = 17;
    pointer[2] = 17;
    pointer[3] = 17;
    pointer[4] = 17;

    auto time1 = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
    InvertBitInPointer(pointer, 5*sizeof(int)-2, {0,1,2,3,4});
    auto time2 = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
    for (int i = 0; i < 5; i++) {
        std::cout << pointer[i] << " ";
    }

    std::cout << "\n";
    InvertBitInPointer(pointer, 5*sizeof(int)-2, 0, 4);
    for (int i = 0; i < 5; i++) {
        std::cout << pointer[i] << " ";
    }

    auto time3 = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
    ParallelInvertBitInPointer(pointer, 5 * sizeof(int), { 0,1,2,3,4 });
    auto time4 = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
    for (int i = 0; i < 5; i++) {
        std::cout << pointer[i] << " ";
    }

    char* str = new char[4];
    str[0] = 'M';
    str[1] = 'a';
    str[2] = 'm';
    str[3] = 'a';
    InvertBitInPointer(str, 4);
    std::cout << str << '\n';

    std::cout << "Parallel " << time4 - time3 << " Not Parallel " << time2 - time1;
}
