#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>

typedef struct msr{
  double timeS;
  int packets;
} Measure;

typedef struct sw_t{
  double sendToCPU;
  double tableUpdate;
} SwitchTimes;

typedef struct tLine{
  double timeS;
  int packetsH1;
  int packetsH2;
  int packetsH3;
  bool miss;
  bool reconfig;
} TableLine;

int intFromString(char str[]){
  int result = 0;

  int ptr = 0;
  while(str[ptr] != '\0'){
    if(str[ptr] >= '0' && str[ptr] <= '9')
      result = result * 10 + (str[ptr] - '0');
    else
      return -1;

    ptr++;
  }

  return result;
}

std::vector<std::string> readFileLines(const std::string& file){
  std::vector<std::string> result;
  std::string line;

  FILE *fp = fopen(file.c_str(), "r");

  if(!fp){
    printf("ERROR: Could not open file \"%s\"\n", file.c_str());
    exit(0);
  }

  while(!feof(fp)){
    char c = fgetc(fp);

    if(c == '\n'){
      if(line.size() > 0){
        result.push_back(line);
      }
      line.clear();
    }
    else{
      line.push_back(c);
    }
  }
  if(line.size() > 1){
    result.push_back(line);
  }

  fclose(fp);

  return result;
}

std::vector<Measure> getMeasures(const std::string& file){
  std::vector<Measure> result;

  std::vector<std::string> fileLines = readFileLines(file);

  for(std::string line : fileLines){
    Measure m;

    char fWord[128];
    char auxWord[128];
    int auxI = 0, auxI2 = 0;

    while(line[auxI] != ' '){
      fWord[auxI] = line[auxI];
      auxI++;
    }
    fWord[auxI+1] = '\0';
    fWord[auxI] = fWord[auxI-1];
    fWord[auxI-1] = fWord[auxI-2];
    fWord[auxI-2] = fWord[auxI-3];
    fWord[auxI-3] = '.';

    m.timeS = atof(fWord);
    if(m.timeS > 10.0){
      int txP = 0, rxP = 0;

      while(line[auxI] != '/')
        auxI++;
      auxI = auxI + 3;

      while(line[auxI] >= '0' && line[auxI] <= '9'){
        auxWord[auxI2] = line[auxI];
        auxI++;
        auxI2++;
      }
      auxWord[auxI2] = '\0';
      txP = intFromString(auxWord);

      auxI2 = 0;

      while(line[auxI] != '/')
        auxI++;
      auxI = auxI + 3;

      while(line[auxI] >= '0' && line[auxI] <= '9'){
        auxWord[auxI2] = line[auxI];
        auxI++;
        auxI2++;
      }
      auxWord[auxI2] = '\0';
      rxP = intFromString(auxWord);

      m.packets = (txP > rxP ? txP : rxP);
      m.packets = (m.packets * 8) / 1048576;

      result.push_back(m);
    }
  }

  return result;
}

double getBaseTime(const std::vector<Measure>& h1){
  int i = 1;
  while(i < (int) h1.size() && h1[i].packets < 10)
    i++;
  if(i == (int) h1.size()){
    printf("ERROR: Could not find base time\n");
    exit(0);
  }

  return floor(h1[i-1].timeS);
}

void fixBaseTime(std::vector<Measure>& h, const double baseTime){
  int p = 0;
  while(h[p].timeS < baseTime)
    p++;

  h.erase(h.begin(), h.begin() + p);

  for(size_t i = 0; i < h.size(); i++)
    h[i].timeS = h[i].timeS - baseTime;
}

int getSwitchBaseTime(const SwitchTimes s1){
  return (int) s1.sendToCPU;
}

void fixSwitchBaseTime(SwitchTimes& s, const int baseTime){
  s.sendToCPU = s.sendToCPU - baseTime;
  s.tableUpdate = s.tableUpdate - baseTime;
}

SwitchTimes getSwitchTimes(const std::string& file){
  SwitchTimes result;
  char auxS[128];
  int auxI = 0;

  std::vector<std::string> fileLines = readFileLines(file);

  int startLine = 0;
  while(fileLines[startLine].find("Action entry is MyIngress.send_to_cpu") == std::string::npos)
    startLine++;

  int auxPt = 0;
  while(fileLines[startLine][auxPt] != '.')
    auxPt++;
  auxPt++;

  auxS[0] = '0';
  auxS[1] = '.';
  for(int i = 0; i < 3; i++)
    auxS[i + 2] = fileLines[startLine][auxPt + i];
  auxS[6] = '\0';
  result.sendToCPU = atof(auxS);

  auxS[2] = '\0';

  auxS[0] = fileLines[startLine][1];
  auxS[1] = fileLines[startLine][2];
  result.sendToCPU = result.sendToCPU + atoi(auxS) * 3600;
  auxS[0] = fileLines[startLine][4];
  auxS[1] = fileLines[startLine][5];
  result.sendToCPU = result.sendToCPU + atoi(auxS) * 60;
  auxS[0] = fileLines[startLine][7];
  auxS[1] = fileLines[startLine][8];
  result.sendToCPU = result.sendToCPU + atoi(auxS) * 1;

  while(fileLines[startLine].find("Entry 1 added to table 'MyIngress.dmac'") == std::string::npos)
    startLine++;

  auxPt = 0;
  while(fileLines[startLine][auxPt] != '.')
    auxPt++;
  auxPt++;

  auxS[0] = '0';
  auxS[1] = '.';
  for(int i = 0; i < 3; i++)
    auxS[i + 2] = fileLines[startLine][auxPt + i];
  auxS[6] = '\0';
  result.tableUpdate = atof(auxS);

  auxS[2] = '\0';

  auxS[0] = fileLines[startLine][1];
  auxS[1] = fileLines[startLine][2];
  result.tableUpdate = result.tableUpdate + atoi(auxS) * 3600;
  auxS[0] = fileLines[startLine][4];
  auxS[1] = fileLines[startLine][5];
  result.tableUpdate = result.tableUpdate + atoi(auxS) * 60;
  auxS[0] = fileLines[startLine][7];
  auxS[1] = fileLines[startLine][8];
  result.tableUpdate = result.tableUpdate + atoi(auxS) * 1;

  return result;
}

bool floatEqs(double f1, double f2){
  return ((f1 - f2) > -0.0001 && (f1 - f2) < 0.0001);
}

void addTableLine(std::vector<TableLine>& table, const TableLine& line){
  TableLine aux;

  table.push_back(line);
  size_t idx = table.size() - 1;

  while(idx > 0 && table[idx].timeS < table[idx-1].timeS){
    aux = table[idx-1];
    table[idx-1] = table[idx];
    table[idx] = aux;

    idx--;
  }

  if(idx > 0 && floatEqs(table[idx].timeS, table[idx-1].timeS)){
    table[idx-1].packetsH1 = table[idx-1].packetsH1 + table[idx].packetsH1;
    table[idx-1].packetsH2 = table[idx-1].packetsH2 + table[idx].packetsH2;
    table[idx-1].packetsH3 = table[idx-1].packetsH3 + table[idx].packetsH3;
    table[idx-1].miss = table[idx-1].miss || table[idx].miss;
    table[idx-1].reconfig = table[idx-1].reconfig || table[idx].reconfig;

    table.erase(table.begin() + idx);
  }
  else if(idx < table.size() - 1 && floatEqs(table[idx].timeS, table[idx+1].timeS)){
    table[idx+1].packetsH1 = table[idx+1].packetsH1 + table[idx].packetsH1;
    table[idx+1].packetsH2 = table[idx+1].packetsH2 + table[idx].packetsH2;
    table[idx+1].packetsH3 = table[idx+1].packetsH3 + table[idx].packetsH3;
    table[idx+1].miss = table[idx+1].miss || table[idx].miss;
    table[idx+1].reconfig = table[idx+1].reconfig || table[idx].reconfig;

    table.erase(table.begin() + idx);
  }
}

size_t sMin(size_t a, size_t b){
	return (a > b) ? b : a;
}

size_t sMax(size_t a, size_t b){
	return (a > b) ? a : b;
}

void fixTableBlanks(std::vector<TableLine>& table){
  int mAnt, mPos;

  std::vector<TableLine> newTable;
  newTable = table;

  const int FIX_RANGE = 5;

  for(size_t i = 0; i < table.size(); i++){
    mAnt = 0;
    for(size_t j = sMax(0, i - FIX_RANGE); j < i; j++)
      if(table[j].packetsH1 > mAnt)
        mAnt = table[j].packetsH1;
    mPos = 0;
    for(size_t j = i; j < sMin(table.size(), i + FIX_RANGE); j++)
      if(table[j].packetsH1 > mPos)
        mPos = table[j].packetsH1;
    newTable[i].packetsH1 = (mPos > mAnt) ? mAnt : mPos;

    mAnt = 0;
    for(size_t j = sMax(0, i - FIX_RANGE); j < i; j++)
      if(table[j].packetsH2 > mAnt)
        mAnt = table[j].packetsH2;
    mPos = 0;
    for(size_t j = i; j < sMin(table.size(), i + FIX_RANGE); j++)
      if(table[j].packetsH2 > mPos)
        mPos = table[j].packetsH2;
    newTable[i].packetsH2 = (mPos > mAnt) ? mAnt : mPos;

    mAnt = 0;
    for(size_t j = sMax(0, i - FIX_RANGE); j < i; j++)
      if(table[j].packetsH3 > mAnt)
        mAnt = table[j].packetsH3;
    mPos = 0;
    for(size_t j = i; j < sMin(table.size(), i + FIX_RANGE); j++)
      if(table[j].packetsH3 > mPos)
        mPos = table[j].packetsH3;
    newTable[i].packetsH3 = (mPos > mAnt) ? mAnt : mPos;
  }

  table = newTable;
}

std::vector<TableLine> genTable(std::vector<Measure>& h1M, std::vector<Measure>& h2M, std::vector<Measure>& h3M,
                                SwitchTimes& s1t, SwitchTimes& s2t, SwitchTimes& s3t)
{
  std::vector<TableLine> table;
  TableLine tempLine;

  tempLine.miss = false;
  tempLine.reconfig = false;
  tempLine.packetsH1 = 0;
  tempLine.packetsH2 = 0;
  tempLine.packetsH3 = 0;
  for(size_t i = 0; i < h1M.size(); i++){
    tempLine.timeS = h1M[i].timeS;
    tempLine.packetsH1 = h1M[i].packets;
    addTableLine(table, tempLine);
  }
  tempLine.packetsH1 = 0;
  for(size_t i = 0; i < h2M.size(); i++){
    tempLine.timeS = h2M[i].timeS;
    tempLine.packetsH2 = h2M[i].packets;
    addTableLine(table, tempLine);
  }
  tempLine.packetsH2 = 0;
  for(size_t i = 0; i < h3M.size(); i++){
    tempLine.timeS = h3M[i].timeS;
    tempLine.packetsH3 = h3M[i].packets;
    addTableLine(table, tempLine);
  }

  tempLine.packetsH3 = 0;
  tempLine.miss = true;
  tempLine.timeS = s1t.sendToCPU;
  addTableLine(table, tempLine);
  tempLine.timeS = s2t.sendToCPU;
  addTableLine(table, tempLine);
  tempLine.timeS = s3t.sendToCPU;
  addTableLine(table, tempLine);

  tempLine.miss = false;
  tempLine.reconfig = true;
  tempLine.timeS = s1t.tableUpdate;
  addTableLine(table, tempLine);
  tempLine.timeS = s2t.tableUpdate;
  addTableLine(table, tempLine);
  tempLine.timeS = s3t.tableUpdate;
  addTableLine(table, tempLine);

  //Removes empty lines
  for(int i = 0; i < (int) table.size(); i++){
    if(table[i].timeS > 0.01 && table[i].packetsH1 == 0 && table[i].packetsH2 == 0 && table[i].packetsH3 == 0 && !table[i].miss && !table[i].reconfig){
      table.erase(table.begin() + i);
      i--;
    }
  }

  fixTableBlanks(table);

  return table;
}

void printTable(const std::vector<TableLine>& table, FILE *fp){
  fprintf(fp, "Tempo;Pacotes H1;Pacotes H2;Pacotes H3;Miss;Reconfig;\n");
  for(TableLine tl : table){
    fprintf(fp, "%.3f;%d;%d;%d;%d;%d\n", tl.timeS, tl.packetsH1, tl.packetsH2, tl.packetsH3, tl.miss ? 120 : 0, tl.reconfig ? 120 : 0);
  }
}

int main(int argc, char *argv[]){
  if(argc != 2){
    printf("Use: %s FOLDER\n", argv[0]);
    return 0;
  }

  std::string folder = std::string(argv[1]);
  if(folder[folder.size() - 1] == '/')
    folder.pop_back();

  std::vector<Measure> h1M = getMeasures(folder + "/h1.txt");
  std::vector<Measure> h2M = getMeasures(folder + "/h2.txt");
  std::vector<Measure> h3M = getMeasures(folder + "/h3.txt");

  double baseTime = getBaseTime(h1M);
  printf("BaseTime: %.3f\n", baseTime);
  fixBaseTime(h1M, baseTime);
  fixBaseTime(h2M, baseTime);
  fixBaseTime(h3M, baseTime);

  /*
  time_t auxT = (time_t) baseTime;

  struct tm * tm = localtime(&auxT);
  printf("%02d:%02d:%02d\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
  */

  printf("- Getting Switch times for s1...\n");
  SwitchTimes s1t = getSwitchTimes(folder + "/s1.log");
  printf("- Getting Switch times for s2...\n");
  SwitchTimes s2t = getSwitchTimes(folder + "/s2.log");
  printf("- Getting Switch times for s3...\n");
  SwitchTimes s3t = getSwitchTimes(folder + "/s3.log");

  double switchBaseTime = getSwitchBaseTime(s1t);
  fixSwitchBaseTime(s1t, switchBaseTime);
  fixSwitchBaseTime(s2t, switchBaseTime);
  fixSwitchBaseTime(s3t, switchBaseTime);

  /*
  printf("S1: %.3f %.3f\n", s1t.sendToCPU, s1t.tableUpdate);
  printf("S2: %.3f %.3f\n", s2t.sendToCPU, s2t.tableUpdate);
  printf("S3: %.3f %.3f\n", s3t.sendToCPU, s3t.tableUpdate);
  */

  printf("- Generating Table...\n");
  std::vector<TableLine> table = genTable(h1M, h2M, h3M, s1t, s2t, s3t);

  FILE *fp = fopen("table.csv", "w");

  printTable(table, fp);

  fclose(fp);

  printf("- Table output to file \"table.csv\"\n");

  return 0;
}
