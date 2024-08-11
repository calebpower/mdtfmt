#include <fstream>
#include <iostream>
#include <string>

using namespace std;

bool isTblDiv(string&);
bool isTblLn(string&);
bool isWhtspc(char&);
void commitBuf(vector<string> &, vector<string> &);

int main(int argc, char** argv) {
  if(2 != argc) {
    cerr << "Please specify a file." << endl;
    return 1;
  }

  ifstream srcFile(argv[1]);
  string line;

  if(!srcFile.is_open()) {
    cerr << "File could not be opened." << endl;
    return 2;
  }

  vector<string> content;
  vector<string> buf;

  bool tblLnActive = false;
  bool tblDvActive = false;

  while(getline(srcFile, line)) {
    bool tla = isTblLn(line);
    bool tda = isTblDiv(line);

    // if this is the first line of a potential table
    if(!tblLnActive && tla) {

      // append the buffer to the content store; clear the buffer
      commitBuf(buf, content);

      tblLnActive = true;
    }

    // if we think we're in a table, but we don't have the table divider
    // then we're really not in a table, so commit the buffer
    if(tblLnActive && 1 == buf.size() && !tda) {
      commitBuf(buf, content);
      tblLnActive = false;
    }

    if(!tla) {

      // and also, if we're no longer in a table, then mark it as such
      // and also go ahead and process the table in the buffer
      if(tblLnActive) {
        for(string line : buf) {
        
        }
      }

      tblLnActive = false;
    }

    // add the current line to the buffer
    buf.push_back(line);

  }

  srcFile.close();

  return 0;
}

bool isTblDiv(string& s) {
  int pipeCnt = 0;
  for(char& c : s)
    if('|' == c)
      pipeCnt++;
    else if('-' != c && !isspace(c))
      return false;
  return 0 < pipeCnt;
}

bool isTblLn(string& s) {
  for(char& c : s) {
    if('|' == c)
      return true;
    else if(!isspace(c))
      return false;
  }
  return false;
}

void commitBuf(vector<string> &src, vector<string> &dest) {
  if(0 < src.size()) {
    dest.insert(end(dest), begin(src), end(src));
    src.clear();
  }
}
