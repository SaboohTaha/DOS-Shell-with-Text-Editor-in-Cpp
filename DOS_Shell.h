#pragma once
#pragma warning(disable : 4996)
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <queue>
#include <stack>
#include <fstream>
#include <Windows.h>

using namespace std;


void gotoRowCol(int rpos, int cpos)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	int xpos = cpos, ypos = rpos;
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = xpos;
	scrn.Y = ypos;
	SetConsoleCursorPosition(hOuput, scrn);
}

// Change this path to your Local Path
string savepath = "E:/OneDrive - Higher Education Commission/Studies/Semester 3/DSA/BSCS20056 DOS Shell/";

class DOS_Shell
{
	struct properties
	{
		time_t time;
		bool isDir;
		bool isHidden;
		int size;
		string name;
	};
	class Editor;
	struct File;
	struct Compare
	{
		bool operator()(const DOS_Shell::File* file1, const DOS_Shell::File* file2)
		{
			return file1->priority < file2->priority;
		}
	};
	struct Dir
	{
		string name;
		Dir* parent;
		list<Dir*> childDir;
		list<File*> files;
		chrono::time_point<chrono::system_clock> created;
		bool isHidden;
		Dir(string _n, Dir* _p = nullptr) :name(_n), parent(_p), created(chrono::system_clock::now()), isHidden(false)
		{
		}
		vector<string> returnPath()
		{
			Dir* cd = parent;
			vector<string> path;
			path.push_back(this->name);
			while (cd)
			{
				path.push_back(cd->name);
				cd = cd->parent;
			}
			for (int i = 0; i < path.size() / 2; i++)
			{
				swap(path[i], path[path.size() - 1 - i]);
			}
			return path;
		}
		void makeDirectory(string name)
		{
			Dir* newDir = new Dir(name, this);
			childDir.push_back(newDir);
		}
		void createFile(string name, string _ext)
		{
			File* newFile = new File(name, this, _ext);
			files.push_back(newFile);
		}
		void loadFile(string str)
		{
			files.push_back(new File(str, this));
		}
		Dir* findDir(string name)
		{
			list<Dir*>::iterator it;
			for (it = childDir.begin(); it != childDir.end(); it++)
			{
				if ((*it)->name == name)
				{
					return *it;
				}
			}
			return nullptr;
		}
		File* findFile(string name, string extension)
		{
			list<File*>::iterator it;
			for (it = files.begin(); it != files.end(); it++)
			{
				if ((*it)->name == name && (*it)->ex == extension)
				{
					return *it;
				}
			}
			return nullptr;
		}
		auto findNoExtFiles(string name)
		{
			vector<File*> filesptr;
			list<File*>::iterator itr;
			for (itr = files.begin(); itr != files.end(); itr++)
			{
				if ((*itr)->name == name)
				{
					filesptr.push_back((*itr));
				}
			}
			return filesptr;
		}
		void showAttrib()
		{
			if (isHidden)
				cout << "H\t";
			else
				cout << " \t";
			vector<string> path = returnPath();
			for (int i = 0; i < path.size(); i++)
			{
				cout << path[i] << ((i == 0) ? ":" : "") << ((i != path.size() - 1) ? '\\' : '\0');
			}
		}
		void changeAttrib(bool hide)
		{
			isHidden = hide;
		}
		properties returnPropeties()
		{
			return { chrono::system_clock::to_time_t(created),true,isHidden,0,name };
		}
		vector<properties> returnDirProperties()
		{
			list<Dir*>::iterator itd;
			list<File*>::iterator itf;
			vector<properties> prop;
			for (itd = childDir.begin(); itd != childDir.end(); itd++)
			{
				prop.push_back((*itd)->returnPropeties());
			}
			for (itf = files.begin(); itf != files.end(); itf++)
			{
				prop.push_back((*itf)->returnPropeties());
			}
			return prop;
		}
		void deleteFile(string name, string ext)
		{
			list<File*>::iterator it;
			for (it = files.begin(); it != files.end(); it++)
			{
				if ((*it)->name == name && (*it)->ex == ext)
				{
					files.erase(it);
					return;
				}
			}
			throw "File does not exists in the Directory";
		}
		void convert(string ext1, string ext2)
		{
			list<File*>::iterator it;
			for (it = files.begin(); it != files.end(); it++)
			{
				if ((*it)->ex == ext1)
					(*it)->ex = ext2;
			}
		}
		void findAllFiles(string input,vector<string>& paths)
		{
			vector<string> nameExt = DOS_Shell::stringToVector(input, ".");
			if (nameExt.size() == 2)
			{
				File* file = findFile(nameExt[0], nameExt[1]);
				if (file)
				{
					vector<string> filepath = file->returnPath();
					string temp;
					for (int i = 0; i < filepath.size(); i++)
					{
						temp += filepath[i] + ((i == 0) ? ":" : "");
						if (i < filepath.size() - 1)
							temp += "\\";
					}
					paths.push_back(temp);
				}
				list<Dir*>::iterator itr;
				for (itr = childDir.begin(); itr != childDir.end(); itr++)
				{
					(*itr)->findAllFiles(input, paths);
				}
			}
			else if (nameExt.size() == 1)
			{
				vector<File*> filesptr = findNoExtFiles(nameExt[0]);
				for (int i = 0; i < filesptr.size(); i++)
				{
					vector<string> filepath = filesptr[i]->returnPath();
					string temp;
					for (int i = 0; i < filepath.size(); i++)
					{
						temp += filepath[i] + ((i == 0) ? ":" : "");
						if (i < filepath.size() - 1)
							temp += "\\";
					}
					paths.push_back(temp);
				}
				list<Dir*>::iterator itr;
				for (itr = childDir.begin(); itr != childDir.end(); itr++)
				{
					(*itr)->findAllFiles(input, paths);
				}
			}
		}
		void findAllString(string input, vector<string>& paths)
		{
			vector<string> filepath;
			string temp;
			for (File* fi : files)
			{
				if (fi->containsString(input))
				{
					filepath = fi->returnPath();
					for (int i = 0; i < filepath.size(); i++)
					{
						temp += filepath[i] + ((i == 0) ? ":" : "");
						if (i < filepath.size() - 1)
							temp += "\\";
					}
					paths.push_back(temp);
				}
			}
			for (Dir* Di : childDir)
			{
				findAllString(input, paths);
			}
		}
	};
	class Editor
	{
		File* open;
		friend class DOS_Shell;
		struct state
		{
			list<list<char>> text;
			int r, c;
		};
		list<list<char>>* text;
		list<list<char>>::iterator rit;
		list<char>::iterator cit;
		int crow, ccol;
		DOS_Shell* dos;
		deque<state> undo;
		stack<state> redo;
	public:
		Editor(File* F, DOS_Shell* D) : open(F), dos(D)
		{
			text = &F->data;
			crow = 0, ccol = 0;
			list<char> temp;
			text->push_back(temp);
			rit = text->begin();
			cit = (*rit).begin();
		}
		void Edit()
		{
			gotoRowCol(crow, ccol);
			int d;
			char ch;
			system("Color F0");
			system("cls");
			Print();
			while (true)
			{
				gotoRowCol(crow, ccol);
				if (_kbhit())
				{
					d = _getch();
					if (d == 224/*Special Characters*/)
					{
						d = _getch();
						if (d == 72/*Up Arrow*/)
						{
							if (crow > 0)
							{
								crow--;
								rit--;
								cit = (*rit).begin();
								if ((*rit).size() <= ccol)
									ccol = (*rit).size();
								advance(cit, ((ccol > 0) ? ccol - 1 : ccol));
							}
						}
						else if (d == 80/*Down Arrow*/)
						{
							if (crow < text->size() - 1)
							{
								crow++;
								rit++;
								cit = (*rit).begin();
								if ((*rit).size() <= ccol)
									ccol = (*rit).size();
								advance(cit, ((ccol > 0) ? ccol - 1 : ccol));
							}
						}
						else if (d == 75/*Left Arrow*/)
						{
							if (ccol > 0)
							{
								ccol--;
								if (ccol != 0)
								{
									cit--;
								}
							}
							else if (ccol == 0 && crow > 0)
							{
								crow--;
								rit--;
								cit = (*rit).begin();
								ccol = (*rit).size();
								advance(cit, ((ccol > 0) ? ccol - 1 : ccol));
							}
						}
						else if (d == 77/*Right Arrow*/)
						{
							if (ccol < (*rit).size())
							{
								if (ccol != 0)
								{
									cit++;
								}
								ccol++;
							}
							else if (crow < text->size() - 1 && ccol == (*rit).size())
							{
								crow++;
								rit++;
								cit = (*rit).begin();
								ccol = 0;
							}
						}
						else if (d == 83/*Delete*/)
						{
							updateUndo();
							Delete();
						}
					}
					else if (d == 13)
					{
						updateUndo();
						Enter();
					}
					else if (d == 14)
					{
						dos->openNewFileEditor(true, open);
						return;
					}
					else if (d == 16)
					{
						dos->openNewFileEditor(false, open);
						return;
					}
					else if (d == 8)
					{
						updateUndo();
						backSpace();
					}
					else if (d == 26)
					{
						undoEditor();
					}
					else if (d == 25)
					{
						Redo_Editor();
					}
					else if (d == 27)
					{
						cout << endl;
						break;
					}
					else
					{
						if (ccol == 0)
						{
							InsertFront(char(d));
						}
						else
						{
							Insert(char(d));
						}
						updateUndo();
					}
				}
			}
			system("cls");
			system("Color 0F");
		}
		void Print()
		{
			for (auto r = text->begin(); r != text->end(); r++)
			{
				for (auto c = (*r).begin(); c != (*r).end(); c++)
				{
					cout << *c;
				}
				cout << endl;
			}
		}
		void Enter()
		{
			auto start = cit, end = cit;
			
			auto temp = rit;
			auto pre_row = rit;
			auto c = cit; 
			if (ccol != 0)
			{
				c++;
				start++;
			}
			rit++;
			text->insert(rit, list<char>());
			rit = ++temp;
			for (; c != (*pre_row).end(); c++)
			{
				(*rit).push_back(*c);
				end++;
			}
			(*pre_row).erase(start, ((ccol == 0) ? end : ++end));
			crow++;
			ccol = 0;
			gotoRowCol(crow, ccol);
			cit = (*rit).begin();
			system("cls");
			Print();
		}
		void Insert(char c)
		{
			auto temp = cit;
			cit++;
			(*rit).insert(cit, c);
			cit = ++temp;

			ccol++;
			printLine();
		}
		void InsertFront(char c)
		{
			(*rit).push_front(c);
			cit = (*rit).begin();
			ccol= 1;
			printLine();
		}
		void backSpace()
		{
			if (ccol == 1)
			{
				cit++;
				(*rit).pop_front();
				printLine();
				ccol--;
			}
			else if (ccol != 0)
			{
				auto temp = cit;
				temp--;
				(*rit).erase(cit);
				cit = temp;
				printLine();
				ccol--;
			}
			else if (ccol == 0 && crow != 0)
			{
				auto temp = rit;
				rit--;
				ccol = (*rit).size();
				(*rit).splice((*rit).end(), (*temp));
				text->erase(temp);
				system("cls");
				Print();
				crow--;
				cit = (*rit).begin();
				advance(cit, ((ccol > 0) ? ccol - 1 : ccol));
			}
		}
		void Delete()
		{
			auto temp = cit;
			//cit++;

			if (ccol == 0 && !(*rit).empty())
			{
				//cit--;
				temp++;
				(*rit).pop_front();
				printLine();
			}
			else if (cit != (*rit).end() && ccol < (*rit).size())
			{
				//ccol++;
				(*rit).erase(++cit);
				//(ccol == 1) ? --cit : cit;
				printLine();
			}
			else
			{
				//ccol++;
			}
			cit = temp;
		}
		void clearLine()
		{
			gotoRowCol(crow, 0);
			cout << "                                                                                                                ";
			gotoRowCol(crow, 0);
		}
		void printLine()
		{
			clearLine();
			for (auto c = (*rit).begin(); c != (*rit).end(); c++)
			{
				cout << *c;
			}
			gotoRowCol(crow, ccol);
		}
		state saveState()
		{
			state* s = new state;
			s->text.push_back(list<char>());
			auto r_itr = s->text.begin();
			for (auto row = text->begin(); row != text->end(); row++)
			{
				if (row != text->begin())
				{
					s->text.push_back(list<char>());
					r_itr++;
				}
				for (auto col = (*row).begin(); col != (*row).end(); col++)
				{
					char ch = *col;
					(*r_itr).push_back(ch);
				}
			}
			s->c = ccol;
			s->r = crow;
			return *s;
		}
		void loadState(state s)
		{
			*text = s.text;
			rit = text->begin();
			ccol = s.c;
			crow = s.r;
			for (int r = 0; r < crow; r++)
			{
				rit++;
			}
			cit = (*rit).begin();
			for (int c = 0; c < ccol - 1; c++)
			{
				cit++;
			}
		}
		void updateUndo()
		{
			static int limit = 5;
			if (undo.size() > limit)
			{
				undo.pop_front();
			}
			state s = saveState();
			undo.push_back(s);
		}
		void undoEditor()
		{
			if (!undo.empty())
			{
				redo.push(saveState());
				loadState(undo.back());
				undo.pop_back();
				system("cls");
				Print();
			}
		}
		void Redo_Editor()
		{
			if (!redo.empty())
			{
				undo.push_back(saveState());
				loadState(redo.top());
				redo.pop();
				system("cls");
				Print();
			}
		}
		~Editor()
		{

		}
	private:
	};
	Dir* root;
	Dir* curr;
	char prompt;
	priority_queue<File*,vector<File*>,DOS_Shell::Compare> pprint;
	deque<File*> qprint;
	static vector<string> stringToVector(string line, string deli)
	{
		vector<string> mult;
		if (line == "")
			return mult;
		string r;
		while (line.find(deli) != string::npos)
		{
			r = line.substr(0, line.find(deli));
			line = line.substr(line.find(deli) + deli.length(), line.size());
			mult.push_back(r);
		}
		mult.push_back(line);
		return mult;
	}
	string to_Lower(string str)
	{
		string res;
		char c;
		for (int i = 0; i < str.length(); i++)
		{
			c = tolower(str.at(i));
			res += c;
		}
		return res;
	}
	Dir* goToPath(vector<string> path, bool lastExc)
	{
		Dir* temp = curr;
		for (int i = 0; i < ((lastExc) ? path.size() - 1 : path.size()); i++)
		{
			temp = temp->findDir(path[i]);
			if (i == 0 && path[i] == root->name)
			{
				temp = root;
			}
			else if (temp == nullptr)
			{
				return temp;
			}
		}
		return temp;
	}
	void coutProperties(Dir* D)
	{
		vector<string> path = curr->returnPath();
		cout << "Directory of ";
		for (int i = 0; i < path.size(); i++)
		{
			cout << path[i] << ((i == 0) ? ":" : "") << prompt;
		}
		cout << endl << endl;
		auto prop = curr->returnDirProperties();
		int dir = 0, files = 0;
		for (int i = 0; i < prop.size(); i++)
		{
			if (!prop[i].isHidden)
			{
				tm dateTime = *localtime(&prop[i].time);
				cout << setfill('0') << setw(2) << right << dateTime.tm_mon + 1 << '/' << 
					setw(2) << right << dateTime.tm_mday << '/' << dateTime.tm_year + 1900
					<< "  " << setw(2) << right << ((dateTime.tm_hour % 12 == 0) ? 12 : dateTime.tm_hour % 12)
					<< ":" << setw(2) << right << dateTime.tm_min << ((dateTime.tm_hour >= 12) ? "  PM" : "  AM")
					<< setfill(' ') << '\t' << setw(14) << ((prop[i].isDir) ? left : right)
					<< ((prop[i].isDir) ? "<DIR>" : to_string(prop[i].size)) << " " << prop[i].name << endl;
			}
			((prop[i].isDir) ? dir++ : files++);
		}
		cout << setw(16) << right << files << " File(s)" << endl;
		cout << setw(16) << right << dir << " Dir(s)" << endl;
	}
	bool isValidFilename(string name)
	{
		for (int i = 0; i < name.size(); i++)
		{
			if (islower(name[i]) || isupper(name[i]) || name.at(i) == '.')
				continue;
			else
				return false;
		}
		return true;
	}
	void saveTreeRecursion(Dir* r, ofstream& fout, int depth)
	{
		if (!r)
			return;
		for (Dir* temp : r->childDir)
		{
			for (int i = 0; i < depth; i++)
			{
				fout << "-" << " ";
			}
			fout << "<DIR> " << temp->name << endl;
			saveTreeRecursion(temp, fout, depth + 1);
		}
		for (File* temp : r->files)
		{
			for (int i = 0; i < depth; i++)
			{
				fout << "-" << " ";
			}
			fout << "<File> " << temp->returnSaveName() << endl;
			temp->saveSelf();
		}
	}
	void loadTreeLoop(Dir* r, ifstream& fin)
	{
		int depth = 0;
		int prev_depth = 0;
		string c;
		string name;
		bool check = false;
		while (fin.good())
		{
			fin >> c;

			if (fin.eof())
				break;
			while (c == "-")
			{
				fin >> c;
				depth++;
				check = true;
			}
			if (check)
			{
				if (depth > prev_depth)
					r = r->findDir(name);
				else if (depth < prev_depth)
					r = r->parent;
			}
			else
			{
				while (r != root)
				{

					r = r->parent;
				}
			}
			if (c == "<DIR>")
			{
				fin >> name;
				r->makeDirectory(name);
				check = false;
			}
			else if (c == "<File>")
			{
				fin >> name;

				r->loadFile(name);
				check = false;
			}
			prev_depth = depth;
			depth = 0;
		}
	}
public:
	struct File
	{
		string name;
		Dir* parent;
		string ex;
		chrono::time_point<chrono::system_clock> modified;
		bool isHidden;
		list<list<char>> data;
		Editor* edit;
		int priority = 0, time = 0;
		File(string _n, Dir* _p, string _e) :name(_n), parent(_p), ex(_e), modified(chrono::system_clock::now()), isHidden(false), edit(nullptr)
		{
		}
		File(const File& f) : name(f.name), parent(f.parent), ex(f.ex), modified(chrono::system_clock::now()), isHidden(f.isHidden), data(f.data), edit(nullptr)
		{
		}
		File(string name, Dir* _p) : parent(_p), modified(chrono::system_clock::now()), isHidden(false), edit(nullptr)
		{
			string path = savepath + name;
			ifstream fin(path);
			char c;
			data.push_back(list<char>());
			auto rit = data.begin();
			while (fin.good())
			{

				fin.get(c);
				if (fin.eof())
					break;
				if (c == '\n')
				{
					data.push_back(list<char>());
					rit++;
				}
				else
				{
					(*rit).push_back(c);
				}
			}
			//Getting Real Name
			auto temp = stringToVector(name, "_");
			auto t1 = stringToVector(temp[temp.size() - 1], ".");
			this->name = t1[0];
			this->ex = t1[1];
		}
		vector<string> returnPath()
		{
			Dir* cd = parent;
			vector<string> path;
			path.push_back(this->name);
			while (cd)
			{
				path.push_back(cd->name);
				cd = cd->parent;
			}
			for (int i = 0; i < path.size() / 2; i++)
			{
				swap(path[i], path[path.size() - 1 - i]);
			}
			path[path.size() - 1] += "." + ex;
			return path;
		}
		string returnNameExtension()
		{
			return name + "." + ex;
		}
		void showAttrib()
		{
			if (isHidden)
				cout << "H\t";
			else
				cout << " \t";
			vector<string> path = returnPath();
			for (int i = 0; i < path.size(); i++)
			{
				cout << path[i] << ((i == 0) ? ":" : "") << ((i != path.size() - 1) ? '\\' : '\0');
			}
		}
		void changeAttrib(bool hide)
		{
			isHidden = hide;
		}
		properties returnPropeties()
		{
			return { chrono::system_clock::to_time_t(modified),false,isHidden,1,returnNameExtension() };
		}
		bool containsString(string str)
		{
			int i = 0;
			for (list<char> f : this->data)
			{
				for (char fc : f)
				{
					if (fc == str[i])
					{
						i++;
						if (i == str.size())
						{
							return true;
						}
					}
					else
					{
						i = 0;
					}
				}
			}
			return false;
		}
		string returnSaveName()
		{
			auto path = returnPath();

			string savename;
			
			for (int i = 0; i < path.size(); i++)
			{
				savename += path[i] + ((i != path.size() - 1) ? "_" : "");
			}
			return savename;
		}
		void saveSelf()
		{
			string completePath = savepath + returnSaveName();
			ofstream fout(completePath);

			for (auto r = data.begin(); r != data.end(); r++)
			{
				for (auto c = (*r).begin(); c != (*r).end(); c++)
				{
					fout << *c;
				}
				fout << endl;
			}
		}
		~File()
		{
			delete edit;
		}
	};
	DOS_Shell(string drive) :prompt('\\')
	{
		curr = root = new Dir(drive);
	}
	void openNewFileEditor(bool isNext, File* F)
	{
		bool isFound = false;
		if (isNext)
		{
			for (list<File*>::iterator fi = curr->files.begin(); fi != curr->files.end();)
			{
				if (isFound)
				{
					A:
					if (!(*fi)->edit)
						(*fi)->edit = new Editor((*fi), this);
					(*fi)->edit->Edit();
					return;
				}
				if ((*fi) == F)
					isFound = true;
				fi++;
				if (fi == curr->files.end())
				{
					fi = curr->files.begin();
					goto A;
				}
			}
		}
		else
		{
			list<File*>::reverse_iterator fi = curr->files.rbegin();
			for (;fi != curr->files.rend();)
			{
				if (isFound)
				{
				B:
					if (!(*fi)->edit)
						(*fi)->edit = new Editor((*fi), this);
					(*fi)->edit->Edit();
					return;
				}
				if ((*fi) == F)
					isFound = true;
				fi++;
				if (fi == curr->files.rend())
				{
					fi = curr->files.rbegin();
					goto B;
				}
			}
		}
	}
	void displayCurrentDirectory()
	{
		vector<string> path = curr->returnPath();
		for (int i = 0; i < path.size(); i++)
		{
			cout << path[i] << ((i == 0) ? ":" : "") << prompt;
		}
	}
	void changeDirectory(string pathOrDir)
	{
		vector<string> path = stringToVector(pathOrDir, "\\");
		try
		{
			if (path.size() == 1)
			{
				auto temp = curr->findDir(path[0]);
				if (temp != nullptr)
					curr = temp;
				else
					throw "Directory Does not Exists!";
			}
			else if (path.size() > 1)
			{
				Dir* temp = curr;
				for (int i = 0; i < path.size(); i++)
				{
					temp = temp->findDir(path[i]);
					if (i == 0 && path[i] == root->name)
					{
						temp = root;
					}
					else if (temp == nullptr)
					{
						throw "One or More Directories Does not exists in the Path!";
					}
				}
				curr = temp;
			}
			else
			{
				cout << "Current Directory is: ";
				displayCurrentDirectory();
			}
		}
		catch (const char* exception)
		{
			cout << "Failed to Change Directory" << endl << exception << endl;
		}
	}
	void displayInitials()
	{
		displayCurrentDirectory();
		cout << ">";
	}
	void changePrompt()
	{
		if (prompt == '\\')
			prompt = '$';
		else
			prompt = '\\';
	}
	void currToParent()
	{
		if (curr->parent)
		{
			curr = curr->parent;
		}
	}
	void currToRoot()
	{
		curr = root;
	}
	void displayHeader()
	{
		cout << "\t\t\t\t\tUser: Muhammad Sabooh Taha" << endl;
		cout << "\t\t\t\t\tRoll Number: BSCS20056" << endl;
		cout << "\t\t\t\t\tCourse Title: DSA" << endl;
	}
	string extractCommand(string& input)
	{
		string res = input.substr(0, input.find(' '));
		if (input.find(' ') != string::npos)
			input = input.substr(input.find(' ') + 1, input.size());
		else
			input.clear();
		return res;
	}
	void makeDirectory(string input)
	{
		vector<string> path = stringToVector(input, "\\");
		try
		{
			if (path[path.size()-1] == "")
			{
				throw "We Can not create a Directory with empty name";
			}
			if (path.size() == 1)
			{
				Dir* temp = curr->findDir(path[path.size() - 1]);
				if (temp == nullptr)
				{
					temp = curr;
					temp->makeDirectory(path[path.size() - 1]);
				}
				else
					throw "Directory with given name already exists";
			}
			else if (path.size() > 1)
			{
				Dir* temp = curr;
				for (int i = 0; i < path.size() - 1; i++)
				{
					temp = temp->findDir(path[i]);
					if (i == 0 && path[i] == root->name)
					{
						temp = root;
					}
					else if (temp == nullptr)
					{
						throw "One or More Directories Does not exists in the Path!";
					}
				}
				if (temp->findDir(path[path.size() - 1]) == nullptr)
					temp->makeDirectory(path[path.size() - 1]);
				else
					throw "Directory with given name already exists in the Destination";
			}
			cout << "Directory made successfully";
		}
		catch (const char* exception)
		{
			cout << "Failed to Make Directory" << endl << exception << endl;
		}
		
	}
	void showAttribute(string input)
	{
		vector<string> path = stringToVector(input, "\\");
		try
		{
			if (path[path.size() - 1] == "")
			{
				throw "Wrong or incomplete input";
			}
			if (path.size() == 1)
			{
				Dir* temp = curr->findDir(path[path.size() - 1]);
				if (temp == nullptr)
				{
					temp = curr;
					vector<string> fname = stringToVector(path[path.size() - 1], ".");
					if (fname.size() == 2)
					{
						File* temp1 = temp->findFile(fname[0], fname[1]);
						if (temp1)
							temp1->showAttrib();
						else
							throw "There exists no file with this name";
					}
					else
						throw "There exists no file or folder with this name";
				}
				else
					temp->showAttrib();
			}
			else if (path.size() > 1)
			{
				Dir* temp = curr;
				for (int i = 0; i < path.size() - 1; i++)
				{
					temp = temp->findDir(path[i]);
					if (i == 0 && path[i] == root->name)
					{
						temp = root;
					}
					else if (temp == nullptr)
					{
						throw "Invalid Path or Filename";
					}
				}
				Dir* prev = temp;
				temp = temp->findDir(path[path.size() - 1]);
				if (temp)
					temp->showAttrib();
				else
				{
					temp = prev;
					vector<string> fname = stringToVector(path[path.size() - 1], ".");
					if (fname.size() == 2)
					{
						File* temp1 = temp->findFile(fname[0], fname[1]);
						if (temp1)
							temp1->showAttrib();
						else
							throw "There exists no file with this name";
					}
					else
						throw "There exists no file or folder with this name";
				}
			}
		}
		catch (const char* exception)
		{
			cout << "Failed to show attribute: " << endl << exception << endl;
		}
	}
	void changeAttribute(string input, bool hide)
	{
		vector<string> path = stringToVector(input, "\\");
		try
		{
			if (path[path.size() - 1] == "")
			{
				throw "Wrong or incomplete input";
			}
			if (path.size() == 1)
			{
				Dir* temp = curr->findDir(path[path.size() - 1]);
				if (temp == nullptr)
				{
					temp = curr;
					vector<string> fname = stringToVector(path[path.size() - 1], ".");
					if (fname.size() == 2)
					{
						File* temp1 = temp->findFile(fname[0], fname[1]);
						if (temp1)
							temp1->changeAttrib(hide);
						else
							throw "There exists no file with this name";
					}
					else
						throw "There exists no file or folder with this name";
				}
				else
					temp->changeAttrib(hide);
			}
			else if (path.size() > 1)
			{
				Dir* temp = goToPath(path,true);
				if (temp == nullptr)
					throw "Invalid Path for File or Directory";
				Dir* prev = temp;
				temp = temp->findDir(path[path.size() - 1]);
				if (temp)
					temp->changeAttrib(hide);
				else
				{
					temp = prev;
					vector<string> fname = stringToVector(path[path.size() - 1], ".");
					if (fname.size() == 2)
					{
						File* temp1 = temp->findFile(fname[0], fname[1]);
						if (temp1)
							temp1->changeAttrib(hide);
						else
							throw "There exists no file with this name";
					}
					else
						throw "There exists no file or folder with this name";
				}
			}
			cout << "Attribute Changed Successfully";
		}
		catch (const char* exception)
		{
			cout << "Failed to change attribute: " << endl << exception << endl;
		}
	}
	void showDirProperties(string input)
	{
		vector<string> path = stringToVector(input, "\\");
		try
		{
			if (input == "")
			{
				coutProperties(curr);
			}
			else if (path.size() > 0)
			{
				Dir* temp = goToPath(path, false);
				if (temp != nullptr)
				{
					coutProperties(temp);
				}
				else
				{
					throw "Invalid Path";
				}
			}
		}
		catch (const char* exception)
		{
			cout << "Unable to Run Command: " << endl << exception;
		}
	}
	void createFile(string input)
	{
		vector<string> nameExt = stringToVector(input, ".");
		try
		{
			if (isValidFilename(input))
			{
				if (nameExt.size() == 2)
				{
					if (nameExt[1] == "txt" || nameExt[1] == "text")
					{
						if (curr->findFile(nameExt[0], "txt") || curr->findFile(nameExt[0], "text"))
						{
							throw "File already exists with the given name";
						}
						curr->createFile(nameExt[0], nameExt[1]);
					}
					else
						throw "The given file extension is not allowed";
				}
				else
					throw "No or Multiple extensions provided";
			}
			else
				throw "Invalid Filename specified";
			cout << "File Created Successfully" << endl;
		}
		catch (const char* exception)
		{
			cout << "Cannot create the File: " << endl << exception << endl;
		}
	}
	void deleteFile(string input)
	{
		vector<string> nameExt = stringToVector(input, ".");
		try
		{
			if (!isValidFilename(input))
				throw "Invalid name for a file";
			if (nameExt.size() != 2)
				throw "None or multiple extensions given";
			curr->deleteFile(nameExt[0], nameExt[1]);
			cout << "File has been deleted Successfully";
		}
		catch (const char* exception)
		{
			cout << "Unable to delete the File: " << endl << exception << endl;
		}
	}
	void convertFiles(string input)
	{
		vector<string> exts = stringToVector(input, ".");
		try
		{
			if (exts.size() != 2)
				throw "Invalid Syntax";
			if (exts[0] != "txt" && exts[0] != "text")
				throw "First file extension not supported";
			if (exts[1] != "txt" && exts[1] != "text")
				throw "Second file extension not supported";
			curr->convert(exts[0], exts[1]);
			cout << "Files extensions converted successfully" << endl;
		}
		catch (const char* exception)
		{
			cout << "Cannot convert files: " << endl << exception << endl;
		}
	}
	void help(string str)
	{
		string input = to_Lower(str);
		if (input == "cd")
		{
			cout << "CD - Changes Directory" << endl;
			cout << "Syntax: CD [Path from current directory or Path from root]" << endl;
			cout << "Alternative Syntax: CD\t\tShows Current Directory" << endl;
		}
		else if (input == "cd.")
		{
			cout << "CD. - Shows Current Directory" << endl;
			cout << "Syntax: CD." << endl;
		}
		else if (input == "cd..")
		{
			cout << "CD.. - Changes Directory to Parent" << endl;
			cout << "Syntax: CD.." << endl;
		}
		else if (input == "cd\\")
		{
			cout << "CD\\ - Changes Directory to the Virtual Drive" << endl;
			cout << "Syntax: CD\\" << endl;
		}
		else if (input == "mkdir")
		{
			cout << "MKDIR - Makes a Directory" << endl;
			cout << "Syntax: MKDIR [Path from current directory or Path from root]\\Dir_Name " << endl;
		}
		else if (input == "md")
		{
			cout << "MD - Makes a Directory" << endl;
			cout << "Syntax: MD [Path from current directory or Path from root]\\Dir_Name " << endl;
		}
		else if (input == "attrib")
		{
			cout << "ATTRIB - Shows or Changes Attributes of a File or Directory" << endl;
			cout << "Syntax: ATTRIB [+/-:H] [Path from current directory or Path from root]\\Name " << endl;
		}
		else if (input == "dir")
		{
			cout << "DIR - Shows List in detail of sub-directories and files" << endl;
			cout << "Syntax: DIR [Path from current directory or Path from root]" << endl;
		}
		else if (input == "create")
		{
			cout << "CREATE - Creates a file in the present directory" << endl;
			cout << "Syntax: CREATE [filename].[extension]" << endl;
			cout << "Only \'.txt\' and \'.text\' extensions are allowed" << endl;
		}
		else if (input == "pwd")
		{
			cout << "PWD - Shows Present Working Directory" << endl;
			cout << "Syntax: PWD" << endl;
		}
		else if (input == "del")
		{
			cout << "DEL - Deletes a file from current directory" << endl;
			cout << "Syntax: DEL [filename].[extension]" << endl;
		}
		else if (input == "ver")
		{
			cout << "VER - Shows version of the program" << endl;
			cout << "Syntax: VER" << endl;
		}
		else if (input == "convert")
		{
			cout << "CONVERT - Converts extensions of all files in present directory" << endl;
			cout << "Syntax: CONVERT [extension1].[extension2]" << endl;
		}
		else if (input == "help")
		{
			cout << "HELP - Shows syntax of a command and its use" << endl;
			cout << "Syntax: HELP [Command_Name]" << endl;
		}
		else if (input == "prompt")
		{
			cout << "PROMPT - Changes Prompt" << endl;
			cout << "Syntax: PROMPT" << endl;
		}
		else if (input == "cls")
		{
			cout << "CLS - Clears Screen" << endl;
			cout << "Syntax: CLS" << endl;
		}
		else if (input == "exit")
		{
			cout << "EXIT - Ends the program" << endl;
			cout << "Syntax: EXIT" << endl;
		}
		else if (input == "copy")
		{
			cout << "COPY - Copies a file and pastes it in the provided destination" << endl;
			cout << "Syntax: COPY [filename].[extension] [Path from current directory or Path from root]" << endl;
		}
		else if (input == "edit")
		{
			cout << "EDIT - Opens the File in Editor" << endl;
			cout << "Syntax: EDIT [filename].[extension]" << endl;
		}
		else if (input == "find")
		{
			cout << "FIND - Finds the File in in the whole Drive" << endl;
			cout << "Syntax: FIND [filename].[extension]" << endl;
		}
		else if (input == "queue")
		{
		cout << "QUEUE - Displays print queue" << endl;
		cout << "Syntax: QUEUE" << endl;
		}
		else if (input == "pqueue")
		{
		cout << "PQUEUE - Displays priority print queue" << endl;
		cout << "Syntax: PQUEUE" << endl;
		}
		else if (input == "print")
		{
		cout << "PRINT - Adds file to the print queue" << endl;
		cout << "Syntax: PRINT [filename].[extension]" << endl;
		}
		else if (input == "pprint")
		{
		cout << "PPRINT - Adds file to the priority print queue" << endl;
		cout << "Syntax: PPRINT  [filename].[extension] [H/L]" << endl;
		}
		else if (input == "findf")
		{
		cout << "FINDF - Finds string in the file" << endl;
		cout << "Syntax: FINDF [filename].[extension] [string]" << endl;
		}
		else if (input == "findstr")
		{
		cout << "FINDSTR - Finds string in the all the files in Drive" << endl;
		cout << "Syntax: FINDSTR [string]" << endl;
		}
		else if (input == "rename")
		{
		cout << "RENAME - Renames the file in present in current directory" << endl;
		cout << "Syntax: RENAME [filename].[extension] [newfilename]" << endl;
		}
		else if (input == "rmdir")
		{
		cout << "RMDIR - Removes directory from a path or current directory" << endl;
		cout << "Syntax: RMDIR [Path from current directory or Path from root]\\Dir_Name" << endl;
		}
		else if (input == "format")
		{
		cout << "FORMAT - Formats the Drive" << endl;
		cout << "Syntax: FORMAT" << endl;
		}
		else if (input == "tree")
		{
		cout << "TREE - Show the tree of the whole Drive" << endl;
		cout << "Syntax: TREE" << endl;
		}
		else if (input == "save")
		{
		cout << "SAVE - Saves the Whole Drive on the Disk" << endl;
		cout << "Syntax: SAVE" << endl;
		}
		else if (input == "load")
		{
		cout << "LOAD - Loads the Drive from the Disk" << endl;
		cout << "Syntax: LOAD" << endl;
		}
		else
		{
			cout << "Command is not recognized" << endl;
		}
	}
	void copyFile(string input)
	{
		vector<string> split = stringToVector(input, " ");
		try
		{
			if (split.size() != 2)
				throw "Invalid Syntax";
			if (!isValidFilename(split[0]))
				throw "Filename is not correct";
			auto nameExt = stringToVector(split[0], ".");
			if (nameExt.size() != 2 || (nameExt[1] != "txt" && nameExt[1] != "text"))
				throw "Invalid Filename";
			File* f1 = curr->findFile(nameExt[0], nameExt[1]);
			if (f1 == nullptr)
				throw "The file does not exits in the current directory";
			auto path = stringToVector(split[1], "\\");
			Dir* Dest = goToPath(path, false);
			if (Dest == nullptr)
				throw "Provided Path is Incorrect";
			File* f2 = Dest->findFile(nameExt[0], "txt");
			if (f2 == nullptr)
				f2 = Dest->findFile(nameExt[0], "text");
			if (f2)
				throw "File with the provided name already exists in destination";
			f2 = new File(*f1);
			Dest->files.push_back(f2);
			cout << "File Copied Successfully" << endl;
		}
		catch (const char* exception)
		{
			cout << "Unable to Copy File: " << endl << exception << endl;
		}
	}
	void editFile(string input)
	{
		vector<string> nameExt = stringToVector(input, ".");
		try
		{
			if (!isValidFilename(input))
				throw "Invalid File name entered";
			if (nameExt.size() != 2)
				throw "No or multiple extensions given";
			File* open = curr->findFile(nameExt[0], nameExt[1]);
			if (open == nullptr)
				throw "The File does not exists in the present directory";
			if (open->edit == nullptr)
			{
				open->edit = new Editor(open,this);
			}
			open->edit->Edit();
		}
		catch (const char* exception)
		{
			cout << "Couldn't open file for editting: " << endl << exception << endl;
		}
	}
	void findFile(string input)
	{
		try
		{
			if (stringToVector(input, " ").size() > 1)
				throw "File name must not contain spaces.";
			vector<string> paths;
			root->findAllFiles(input, paths);
			if (paths.size() == 0)
				throw "File does not exists in the Virtual Directory.";
			cout << "File(s) Found:" << endl;
			for (int i = 0; i < paths.size(); i++)
			{
				cout << paths[i] << endl;
			}
			cout << endl << "Total Files found in Drive: " << paths.size() << endl;
		}
		catch (const char* exception)
		{
			cout<<"Couldn't find file: " << endl << exception << endl;
		}
	}
	void displayDriveTree(Dir* r, int depth)
	{
		if (!r)
			return;
		for (Dir* temp : r->childDir)
		{
			cout << "|";
			for (int i = 0; i < depth; i++)
			{
				cout << "\t";
			}
			cout << "|--";
			cout << temp->name << endl;
			displayDriveTree(temp, depth + 1);
		}

		for (File* temp : r->files)
		{
			cout << "|";
			for (int i = 0; i < depth; i++)
			{
				cout << "\t";
			}
			cout << "|--";
			cout << temp->name << "." << temp->ex << endl;
		}
	}
	void insertInQueue(string input)
	{
		try
		{
			if (stringToVector(input, " ").size() > 1)
				throw "Only Filename is required for inserting in Print Queue.";
			vector<string> nameExt = stringToVector(input, ".");
			if (nameExt.size() > 2)
				throw "Invalid Filename Syntax.";
			File* fi = curr->findFile(nameExt[0], nameExt[1]);
			if (!fi)
				throw "File not found in the Current Directory.";
			qprint.push_back(fi);
			fi->time = 50;
			cout << "File Added to the Print Queue" << endl;
		}
		catch (const char* exception)
		{
			cout << "Couldn't Add File to the Print Queue: " << endl << exception << endl;
		}
	}
	void insertInPriorityQueue(string input)
	{
		try
		{
			vector<string> nameP = stringToVector(input, " ");
			if (nameP.size() != 2)
				throw "Filename and its Priority is required for inserting in Priority Print Queue.";
			vector<string> nameExt = stringToVector(nameP[0], ".");
			if (nameExt.size() > 2)
				throw "Invalid Filename Syntax.";
			File* fi = curr->findFile(nameExt[0], nameExt[1]);
			if (!fi)
				throw "File not found in the Current Directory.";
			if (nameP[1] == "H" || nameP[1] == "h")
			{
				fi->priority = 1;
			}
			else if (nameP[1] == "L" || nameP[1] == "l")
			{
				fi->priority = 0;
			}
			else
			{
				cout << "Priority not specified correctly. Low[L] Assumed" << endl;
			}
			pprint.push(fi);
			fi->time = 50;
			cout << "File Added to the Priority Print Queue" << endl;
		}
		catch (const char* exception)
		{
			cout << "Couldn't Add File to the Priority Print Queue: " << endl << exception << endl;
		}
	}
	void displayQueue()
	{
		static int time = 5;
		cout << endl << setw(40) << left << "Filename" << setw(0) << left << "Time" << endl;
		if (qprint.empty())
			return;
		auto print_file = qprint.front();
		print_file->time -= time;
		if (print_file->time <= 0)
		{
			print_file->time = 0;
			qprint.pop_front();
		}
		string t1;
		for (File* temp : qprint)
		{
			t1 = temp->name + "." + temp->ex;
			cout << setw(40) << left << t1 << setw(0) << left << temp->time << " seconds" << endl;
		}
		cout << endl;
	}
	void displayProrityQueue()
	{
		static int time = 5;
		cout << endl << setw(50) << left << "Filename" << setw(0) << left << "Time" << endl;
		if (pprint.empty())
			return;
		auto print_file = pprint.top();
		print_file->time -= time;
		if (print_file->time <= 0)
		{
			print_file->time = 0;
			pprint.pop();
		}
		auto temp = pprint;
		string t1;
		while (temp.size() != 0)
		{
			t1 = temp.top()->name + "." + temp.top()->ex;
			cout << setw(50) << left << t1 << setw(0) << left << temp.top()->time << " seconds" << endl;
			temp.pop();
		}
		cout << endl;
	}
	void findStringInFile(string input)
	{
		try
		{
			vector<string> nameExt = stringToVector(extractCommand(input), ".");
			if (nameExt.size() != 2)
				throw "Invalid file format";
			File* fi = curr->findFile(nameExt[0], nameExt[1]);
			if (!fi)
				throw "File not present in current directory";
			if (fi->containsString(input))
			{
				cout << "String Found in the File" << endl;
			}
			else
			{
				cout << "String NOT Found in the File" << endl;
			}
		}
		catch (const char* exception)
		{
			cout << "Couldn't start search: " << endl << exception << endl;
		}
	}
	void findStringInDrive(string input)
	{
		vector<string> paths;
		root->findAllString(input, paths);
		if (paths.size() == 0)
		{
			cout << "String Not Found in any file" << endl;
			return;
		}
		cout << "String Found in Following Files: " << endl << endl;
		for (int i = 0; i < paths.size(); i++)
		{
			cout << paths[i] << endl;
		}
		cout << endl;
		cout << "String Found in File(s): " << paths.size() << endl;
	}
	void rename(string input)
	{
		try
		{
			vector<string> names = stringToVector(input, " ");
			if (names.size() != 2)
				throw "Correct Rename Syntax not followed";
			vector<string> nameExt = stringToVector(names[0],".");
			if (nameExt.size() != 2)
				throw "File name to rename is incorrect";
			File* fi = curr->findFile(nameExt[0], nameExt[1]);
			if (!fi)
				throw "File to rename is not present in the current directory";
			nameExt = stringToVector(names[1], ".");
			if (nameExt.size() > 1)
				throw "Only Filename should be provided not extension";
			if (!isValidFilename(nameExt[0]))
				throw "Invalid New File Name";
			fi->name = nameExt[0];
			cout << "File renamed successfully" << endl;
		}
		catch (const char* exception)
		{
			cout << "Couldn't rename the file: " << endl << exception << endl;
		}
	}
	void removeDirectory(string input)
	{
		try
		{
			vector<string> path = stringToVector(input, "\\");
			Dir* di = goToPath(path, false);
			if (!di)
				throw "Directory With Given Name Not Found";
			di->parent->childDir.remove(di);
			cout << "Directory Removed Successfully";
		}
		catch (const char* exception)
		{
			cout << "Couldn't Remove the Directory: " << endl << exception << endl;
		}
	}
	void formatDrive()
	{
		string name = root->name;
		delete root;
		curr = root = new Dir(name);
	}
	void saveTree()
	{
		string name = "tree.txt";
		string path = savepath + name;
		ofstream fout(path);
		saveTreeRecursion(root, fout, 0);
		cout << "Tree Saved Successfully" << endl;
	}
	void loadTree()
	{
		string name = "tree.txt";
		string path = savepath + name;
		ifstream fin(path);
		formatDrive();
		loadTreeLoop(root, fin);
		curr = root;
	}
	void Main()
	{
		system("cls");
		displayHeader();
		string input;
		string command;
		system("Color 0F");
		while (true)
		{
			displayInitials();
			getline(cin, input);
			command = extractCommand(input);
			command = to_Lower(command);
			if (command == "cd")
			{
				changeDirectory(input);
				cout << endl;
			}
			else if (command == "cd.")
			{
				cout << "Curent Working Directory is: ";
				displayCurrentDirectory();
				cout << endl;
			}
			else if (command == "cd..")
			{
				currToParent();
			}
			else if (command == "cd\\")
			{
				currToRoot();
			}
			else if (command == "mkdir" || command == "md")
			{
				makeDirectory(input);
				cout << endl;
			}
			else if (command == "attrib")
			{
				vector<string> attrib = stringToVector(input, " ");
				if (attrib.size() == 2 && (to_Lower(attrib[0]) == "+h" || to_Lower(attrib[0]) == "-h"))
					changeAttribute(attrib[1], ((to_Lower(attrib[0]) == "+h") ? true : false));
				else if (attrib.size() == 1)
					showAttribute(attrib[0]);
				else
					cout << "Invalid Input";
				cout << endl;
			}
			else if (command == "dir")
			{
				showDirProperties(input);
				cout << endl;
			}
			else if (command == "create")
			{
				createFile(input);
				cout << endl;
			}
			else if (command == "pwd")
			{
				cout << "Present Working Directory is: ";
				displayCurrentDirectory();
				cout << endl;
			}
			else if (command == "ver")
			{
				cout << "Current version of the program is 1.0.2" << endl;
			}
			else if (command == "del")
			{
				deleteFile(input);
				cout << endl;
			}
			else if (command == "convert")
			{
				convertFiles(input);
				cout << endl;
			}
			else if (command == "help")
			{
				help(input);
				cout << endl;
			}
			else if (command == "copy")
			{
				copyFile(input);
				cout << endl;
			}
			else if (command == "edit")
			{
				editFile(input);
				cout << endl;
			}
			else if (command == "find")
			{
				findFile(input);
				cout << endl;
			}
			else if (command == "print")
			{
				insertInQueue(input);
				cout << endl;
			}
			else if (command == "pprint")
			{
				insertInPriorityQueue(input);
				cout << endl;
			}
			else if (command == "queue")
			{
				displayQueue();
				cout << endl;
			}
			else if (command == "pqueue")
			{
				displayProrityQueue();
				cout << endl;
			}
			else if (command == "findf")
			{
			findStringInFile(input);
			cout << endl;
			}
			else if (command == "findstr")
			{
			findStringInDrive(input);
			cout << endl;
			}
			else if (command == "rename")
			{
			rename(input);
			cout << endl;
			}
			else if (command == "rmdir")
			{
			removeDirectory(input);
			cout << endl;
			}
			else if (command == "format")
			{
			formatDrive();
			cout << endl;
			}
			else if (command == "tree")
			{
				cout << root->name << endl;
				displayDriveTree(root, 0);
				cout << endl;
			}
			else if (command == "save")
			{
			saveTree();
			cout << endl;
			}
			else if (command == "load")
			{
			loadTree();
			cout << endl;
			}
			else if (command == "prompt")
			{
				changePrompt();
				cout << endl;
			}
			else if (command == "cls")
			{
				system("cls");
				displayHeader();
			}
			else if (command == "exit")
			{
				break;
			}
		}
	}
	~DOS_Shell()
	{
		formatDrive();
		delete root;
	}
};

