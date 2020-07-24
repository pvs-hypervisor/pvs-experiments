#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>

using namespace std;

int intFromStr(char *str){
  int result = 0;
  while(*str != '\0'){
    result = result * 10 + (*str - '0');
    str++;
  }
  return result;
}

vector<string> readFileLines(string filename){
	ifstream istr;
	string line;
	vector<string> lines;

	istr.open(filename.c_str());

	if(!istr)
		return lines;

	while(!istr.eof()){
		getline(istr,line);
		lines.push_back(line);
	}

	istr.close();

	return lines;
}

int main(int argc, char* argv[]){
  if(argc < 2){
    printf("Wrong Arguments\n");
    return 0;
  }

  bool scale = false;
  if(argc > 2 && !strcmp("--scale", argv[2])) scale = true;

  vector<string> fileLines = readFileLines(argv[1]);

  long long int injectionMedia[4];

  int i = 0;
  int j = 0;
  int k = 0;

  int soma, count;

  char aux[2000];

  for(int x = 0; x < 4; x++){
    while(fileLines[i].find("Count") == string::npos)
      i++;

    soma = 0;
    count = 0;
    while(fileLines[i].find("Count") != string::npos){
      j = 0;
      while(fileLines[i][j] <= '0' || fileLines[i][j] >= '9')
        j++;
      k = 0;
      while(fileLines[i][j] >= '0' && fileLines[i][j] <= '9'){
        aux[k] = fileLines[i][j];
        k++;
        j++;
      }
      aux[k] = '\0';
      soma = soma + intFromStr(aux);
      count++;
      i++;
    }
    int media = soma/count;

    if(scale) media = media / 1.2178;

    injectionMedia[x] = media;
    printf("%lld\n", injectionMedia[x] * 2048);
  }
  printf("%lld\n", injectionMedia[3] * 2048);

  return 0;
}
