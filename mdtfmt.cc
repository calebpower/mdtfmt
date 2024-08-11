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

        // go ahead and instantiate our column counter
        int maxCols = 0;
        for(int i = 0; i < buf.size(); i++) {
          if(1 == i) continue;
          int sz = countCols(buf[i], '|');
          if(sz > maxCols)
            maxCols = sz;
        }

        // make an array, [rows] x [columns]; this will be our table
        string tblArr[buf.size()][maxCols];
        int colSzs[maxCols]; // keep track of the column sizes
        char colAligns[maxCols]; // and keep track of column alignments
        for(size_t i = 0; i < maxCols; i++) {
          colSzs[i] = 0; // make sure to zero out column sizes
          colAligns[i] = (char)0x0; // and zero out column alignments
        }

        for(size_t i = 0; i < buf.size(); i++) { // now, for each row
          vector<string> row = splitCols(buf[i]); // get the tokens of that row
          for(size_t j = 0; j < row.size() && j < maxCols; j++) { // now for each cell in that row
            trim(row[j]); // trim the cell down
            tblArr[i][j] = ' ' + row[j] + ' '; // pre-pad the cell and put it in the correct col/row
            if(1 == i) { // this is the divider
              if(0 < row[j].size()) {
                if(':' == row[j][0]) // left or center aligned (check first char)
                  colAligns[j] |= 0xF0;
                if(':' == row[j][row[j].size() - 1]) // right or center aligned (check last char)
                  colAligns[j] |= 0x0F;
              }
            } else if(colSzs[j] < row[j].size() + 2) {
              colSzs[j] = row[j].size() + 2;
            }
          }
          for(size_t j = row.size(); j < maxCols; j++) { // empty cells should be blank
            tblArr[i][j] = ""; // empty
          }
        }

        // XXX
        cout << endl;
        for(size_t i = 0; i < maxCols; i++) {
          cout << colSzs[i] << " ";
        }
        cout << endl << endl;

        // here, we're going to go ahead and take a look at alignment
        const size_t minDashes = 3; // we need to start off with at least three dashes ("---")
        for(size_t i = 0; i < maxCols; i++) { // for each column
          bool lc = colAligns[i] & 0xF0;
          bool rc = colAligns[i] & 0x0F;
          string align = "";

          for(size_t j = 0; j < (((3 > (colSzs[i] - lc - rc)) ? minDashes : (colSzs[i])) - lc - rc); j++)
            align += "-"; // but add more as necessary

          if(lc) align = ":" + align; // check for and apply left/center alignment
          if(rc) align += ":";        // check for and apply right/center alignment

          // now make sure we don't need to up the max size of the column
          if(colSzs[i] < align.size() - 2)
            colSzs[i] = align.size() - 2;
          else if(colSzs[i] < minDashes)
            colSzs[i] = minDashes;

          tblArr[1][i] = align; // make sure to save the text
        }

        // now we should right-pad those values that need it
        for(size_t i = 0; i < buf.size(); i++) { // rows
          if(1 == i) continue;
          for(size_t j = 0; j < maxCols; j++) // columns
            while(tblArr[i][j].size() < colSzs[j])
              tblArr[i][j] += " ";
        }

        // XXX we're just print stuff out here

        cout << endl;
        for(size_t i = 0; i < maxCols; i++) {
          cout << colSzs[i] << " ";
        }
        cout << endl << endl;

        for(size_t i = 0; i < buf.size(); i++) { // rows
          for(size_t j = 0; j < maxCols; j++) { // columns
            cout << "[" << tblArr[i][j] << "]\t";
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
    else if('-' != c && ':' != c && !isspace(c))
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
