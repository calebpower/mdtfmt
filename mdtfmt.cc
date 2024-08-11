#include <fstream>
#include <iostream>
#include <string>

using namespace std;

bool isTblDiv(string&);
bool isTblLn(string&);
bool isWhtspc(const char&);
void commitBuf(vector<string>&, vector<string>&);
int countCols(const string&, const char);
vector<string> splitCols(const string&);
void trim(string &s);

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

        int maxCols = 0;
        for(string line : buf) {
          int mp = countCols(line, '|');
          if(mp > maxCols)
            maxCols = mp;
        }

        cout << "max cols is " << maxCols << endl;

        string tblArr[maxCols][buf.size()];
        int colSzs[maxCols];
        for(size_t i = 0; i < maxCols; i++)
          colSzs[i] = 0;

        for(size_t i = 0; i < buf.size(); i++) {
          vector<string> row = splitCols(buf[i]);
          for(size_t j = 0; j < row.size() && j < maxCols; j++) {
            trim(row[j]);
            tblArr[j][i] = row[j];
            if(colSzs[j] < row[j].size())
              colSzs[j] = row[j].size();
          }
          for(size_t j = row.size(); j < maxCols; j++) {
            tblArr[j][i] = "";
          }
        }

        cout << endl;
        for(size_t i = 0; i < maxCols; i++) {
          cout << colSzs[i] << " ";
        }
        cout << endl << endl;

        for(size_t i = 0; i < buf.size(); i++) {
          for(size_t j = 0; j < maxCols; j++) {
            cout << "[" << tblArr[j][i] << "]\t";
          }
          cout << endl;
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

int countCols(const string &haystack, const char needle) {
  int count = 0;
  bool esc = false;
  bool found = false;
  bool end = false;

  for(char c : haystack) {
    if(!isspace(c))
      end = false;

    if(esc)
      esc = false;
    else if('\\' == c)
      esc = true;
    else if(needle == c) {
      ++count;
      end = true;
    }

/*
    if(!found) {
      if('|' == c) {
        found = true;
      } else if(!isspace(c)) {
        count++;
        found = true;
      }
    }
*/

  }

  if(!end) count++;
  return count - 1;
}

vector<string> splitCols(const string &s) {
  vector<string> res;
  string curr;
  bool esc = false;
  bool trimmed = false;

  for(size_t i = 0; i < s.length(); i++) {
    if(esc) {
      curr += s[i];
      esc = false;
    } else if('\\' == s[i]) {
      esc = true;
      curr += s[i];
    } else if('|' == s[i]) {
      if(trimmed)
        res.push_back(curr);
      else trimmed = true;
      curr.clear();
    } else {
      curr += s[i];
    }
  }

  if(trimmed)
    res.push_back(curr);
  else trimmed = true;

  return res;
}

void trim(string &s) {
  s.erase(
      s.begin(),
      find_if(s.begin(), s.end(), [](unsigned char c) {
        return !isspace(c);
      }));
  s.erase(
      find_if(s.rbegin(), s.rend(), [](unsigned char c) {
        return !isspace(c);
      }).base(), s.end());
}
