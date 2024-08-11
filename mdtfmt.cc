/*
* Copyright (c) 2024 Caleb L. Power.
*/

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
void tryPad(const string&);
void processTbl(vector<string>&, vector<string>&, const string&);

int main(int argc, char** argv) {
  if(2 != argc) {
    cerr << "Usage: " << argv[0] << " <input>" << endl;
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

  // read the file
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
      if(tblLnActive)
        processTbl(buf, content, line);

      tblLnActive = false;
    }

    // add the current line to the buffer
    buf.push_back(line);
  }

  // process residual table if it's still in the buffer
  if(tblLnActive)
    processTbl(buf, content, line);

  // flush the buffer
  for(size_t i = 0; i < buf.size(); i++) {
    cout << buf[i] << endl;
  }

  // close out the file
  srcFile.close();

  // so long, and thanks for all the fish!
  return 0;
}

// format the buffer into a prettified markdown table
void processTbl(vector<string> &buf, vector<string> &content, const string &line) {
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

  // start calculating the sizes needed for the columns
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

  // here, we're going to go ahead and take a look at alignment
  const size_t minDashes = 3; // we need to start off with at least three dashes ("---")
  for(size_t i = 0; i < maxCols; i++) { // for each column
    bool lc = colAligns[i] & 0xF0;
    bool rc = colAligns[i] & 0x0F;
    string align = "";

    // figure out how many dashes we're going to need for the divider row
    for(size_t j = 0; j < (((minDashes > colSzs[i] - lc - rc) ? minDashes : colSzs[i]) - lc - rc); j++)
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

  // we're just printing stuff out here
  for(size_t i = 0; i < content.size(); i++) {
    cout << content[i] << endl;
  }
  if(0 < content.size()) // need to make sure a blank row exists before the table
    tryPad(content[content.size() - 1]);
  content.clear();

  for(size_t i = 0; i < buf.size(); i++) { // rows
    cout << "|"; // leading pipe
    for(size_t j = 0; j < maxCols; j++) { // columns
      cout << tblArr[i][j] << "|"; // content
    }
    cout << endl;
  }

  buf.clear();

  tryPad(line); // need to make sure a blank row exists at the end of the table
}

// determines whether or not the string is likely a table divider (i.e. the alignment row);
// basically, any string that has at least one pipe and only contains pipes, dashes,
// colons, or whitespace characters is considered a divider
bool isTblDiv(string& s) {
  int pipeCnt = 0;
  for(char& c : s)
    if('|' == c)
      pipeCnt++;
    else if('-' != c && ':' != c && !isspace(c))
      return false;
  return 0 < pipeCnt;
}

// determines whether or not the string is likely a row in the table; essentially,
// any line that starts with a pipe character (optionally prepended with whitespace)
// is considered a table line
bool isTblLn(string& s) {
  for(char& c : s) {
    if('|' == c)
      return true;
    else if(!isspace(c))
      return false;
  }
  return false;
}

// appends the source vector to the destination vector, and then wipes the
// source vector
void commitBuf(vector<string> &src, vector<string> &dest) {
  if(0 < src.size()) {
    dest.insert(end(dest), begin(src), end(src));
    src.clear();
  }
}

// counts columns in a row; basically, the needle is considered the delimeter
// (and should be a pipe for markdown tables); with consideration for the
// standard slash-driven escape mechanism, only unescaped pipes are counted;
// a well-formed table is going to have one fewer column than there are pipes;
// however, we're also allowing for when the author forgets to add a trailing
// pipe, in which case the number of columns will match the number of pipes
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
  }

  if(!end) count++;
  return count - 1;
}

// here, we're taking a string and splitting it up into a vector delimited by
// unescaped pipes (and we're using the standard slash-driven escape mechanism)
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

// standard trim operation--remove whitespace from the beginning and end of a string
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

// if the provided string has any non-whitespace characters, spit out a newline
void tryPad(const string &s) {
  for(size_t i = 0; i < s.size(); ++i)
    if(!isspace(s[i])) {
      cout << endl;
      break;
    }
}
