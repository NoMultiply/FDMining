#pragma once

#include <vector>
//��ϵ������Ͷ��壬��һά��ʾ��ͬ���ԣ��ڶ�ά��ʾ��ͬԪ��
typedef std::vector<std::vector<std::string>> RTable;

//�������������Ͷ��壬first��ʾ����������ߵ����Լ��ϣ�second��ʾ���������ұߵ�����
typedef std::pair<std::vector<int>, int> FDUnit;

//������������
typedef std::vector<FDUnit> FDSet;

bool gb_loadDataFromFile(std::string filename, RTable & r);

void gb_runMinerWithTimeCost(void miner(RTable &, FDSet &), RTable & r, FDSet & FDs);

bool gb_writeResultToFile(std::string filename, FDSet & FDs, bool printOnConsole = true);
