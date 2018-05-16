#pragma once

#include <vector>
//关系表的类型定义，第一维表示不同属性，第二维表示不同元组
typedef std::vector<std::vector<std::string>> RTable;

//函数依赖的类型定义，first表示函数依赖左边的属性集合，second表示函数依赖右边的属性
typedef std::pair<std::vector<int>, int> FDUnit;

//函数依赖集合
typedef std::vector<FDUnit> FDSet;

bool gb_loadDataFromFile(std::string filename, RTable & r);

void gb_runMinerWithTimeCost(void miner(RTable &, FDSet &), RTable & r, FDSet & FDs);

bool gb_writeResultToFile(std::string filename, FDSet & FDs, bool printOnConsole = true);
