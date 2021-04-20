/*
COP3530 Final Project
Using imdb dataset to create a movie similarity prediction app
Authors: Adeeb Rashid, Caroline Rogers, Zachary Speaks
*/


#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <queue>
#include <chrono>
#include <ctime>

//#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/text_oarchive.hpp>

using namespace std;
class Vertex {

public:
	string title;
	int isAdult;
	int year;
	int runtime;
	vector<string> genres;
	unordered_map<string,pair<Vertex*, double>> edges;

	Vertex() {
		this->title = "";
		this->runtime = -1;
	}
	Vertex(string _title) {
		this->title = _title;
		this->runtime = 0;
	}
	Vertex(string _title, string _isAdult, string _year, string _runtime, vector<string> _genres) {
		
		this->title = _title;
		this->genres = _genres;
		
		try {
			this->isAdult = stoi(_isAdult);
		}
		catch (...) {
			this->isAdult = -1;
		}
		try {
			this->year = stoi(_year);
		}
		catch (...) {
			this->year = -1;
		}
		try {
			this->runtime = stoi(_runtime);
		}
		catch (...) {
			this->runtime = -1;
		}
	}
	
};

struct compareFreq {
	bool operator()(pair<double, string> n1, pair<double, string> n2) {
		double f1 = n1.first;
		double f2 = n2.first;
		return f1 < f2;
	}
};

class Graph {

public:
	unordered_map<string, Vertex*> allMovies;
	unordered_map<string, string> movieNames;

	int numVertices;

	Graph() {
		this->numVertices = 0;
	}
	Graph(int num) { this->numVertices = num; }

	void addVertex(string _tid, Vertex* _vert) {
		this->allMovies.insert({ _tid, _vert });
		this->numVertices += 1;
	}
	void readfile(string filepath) {
	ifstream file(filepath);
	int count = 0;
	if (file.is_open()) {
		string line;
		getline(file, line);

		string _tid;
		string _title;
		string _isAdult;
		string _year;
		string _runtime;
		string genres;
		string trash;

		string delimiter = "\t";

		while (getline(file, line)) {
			count++;
			if (count % 500 == 0) { cout << count << endl; }
			//A:tid
			_tid = line.substr(0, line.find(delimiter));
			line = line.substr(line.find(delimiter) + 1, line.length() - line.find(delimiter) + 1);
			//B:trash
			trash = line.substr(0, line.find(delimiter));
			line = line.substr(line.find(delimiter) + 1, line.length() - line.find(delimiter) + 1);
			//C:title
			_title = line.substr(0, line.find(delimiter));
			line = line.substr(line.find(delimiter) + 1, line.length() - line.find(delimiter) + 1);
			//D:trash
			trash = line.substr(0, line.find(delimiter));
			line = line.substr(line.find(delimiter) + 1, line.length() - line.find(delimiter) + 1);
			//E:isAdults
			_isAdult = line.substr(0, line.find(delimiter));
			line = line.substr(line.find(delimiter) + 1, line.length() - line.find(delimiter) + 1);
			//F:year
			_year = line.substr(0, line.find(delimiter));
			line = line.substr(line.find(delimiter) + 1, line.length() - line.find(delimiter) + 1);
			//G:trash
			trash = line.substr(0, line.find(delimiter));
			line = line.substr(line.find(delimiter) + 1, line.length() - line.find(delimiter) + 1);
			//H:runtime
			_runtime = line.substr(0, line.find(delimiter));
			line = line.substr(line.find(delimiter) + 1, line.length() - line.find(delimiter) + 1);
			//I:genre
			genres = line.substr(0, line.find(delimiter));
			line = line.substr(line.find(delimiter) + 1, line.length() - line.find(delimiter) + 1);

			vector<string> _genres;

			string currentGenre;

			while (genres.find(",") != -1) {
				string delim = ",";
				currentGenre = genres.substr(0, genres.find(delim));
				genres = genres.substr(genres.find(delim) + 1, genres.length() - genres.find(delim) + 1);
				_genres.push_back(currentGenre);
			}
			currentGenre = genres;
			_genres.push_back(currentGenre);

			Vertex* movie = new Vertex(_title, _isAdult, _year, _runtime, _genres);

			this->addVertex(_tid, movie);
			movieNames[_title] = _tid;
		}
	}
}
	void addEdge(string filepath) {
	ifstream file(filepath);
	int count = 0;
	if (file.is_open()) {
		string line;
		string trash;
		string delimiter = "\t";
		string delim = ",";
		string movie;
		while (getline(file, line)) {
			count++;
			if (count % 500 == 0) { cout << count << endl; }
			//A:trash
			trash = line.substr(0, line.find(delimiter));
			//List of movies
			line = line.substr(line.find(delimiter) + 1, line.length() - line.find(delimiter) + 1);

			//error
			line = line.substr(1, line.size() - 2);
			vector<string> career;

			while (line.find(",") != -1) {
				movie = line.substr(1, line.find(delim) - 2);
				line = line.substr(line.find(delim) + 2, line.length() - line.find(delim) + 1);
				career.push_back(movie);
			}
			line = line.substr(1, line.size() - 2);
			career.push_back(line);

			for (int i = 0; i < career.size(); i++) {
				for (int j = i + 1; j < career.size(); j++) {
					if (allMovies.find(career[i]) != allMovies.end()) {
						if (allMovies.find(career[j]) != allMovies.end()) {
							if (allMovies[career[i]]->edges.find(career[j]) == allMovies[career[i]]->edges.end()) {
								pair<Vertex*, double> newEdge(allMovies[career[j]],1);
								allMovies[career[i]]->edges[career[j]] = newEdge;

								pair<Vertex*, double> newEdge2(allMovies[career[i]], 1);
								allMovies[career[j]]->edges[career[i]] = newEdge2;
							}
							else {
								allMovies[career[i]]->edges[career[j]].second += 1;
								allMovies[career[j]]->edges[career[i]].second += 1;
							}

						}
					}
				}
			}
		}
	}
}

	void printVertices() {
		for (auto iter : allMovies) {
			cout << "tid: " << iter.first << " title: " << iter.second->title << " isAdult: " << iter.second->isAdult << " year: " << iter.second->year << " runtime: " << iter.second->runtime << endl;
			cout << "genres: " << endl;
			for (auto it : iter.second->genres) {
				cout << it << endl;
			}
			for (auto it : iter.second->edges) {
				cout << it.first << " " << it.second.second << endl;
			}
			cout << endl;

		}
	}
	double calculateSIB(Vertex* v1, Vertex* v2, double edgeWeight, int depth) {

		double Adult = 1;
		double Runtime = 3;
		double Genre = 1;
		double People = edgeWeight;
		double Seperations = depth;
		double SI = 1;

		if (v1->isAdult != -1 && v2->isAdult != -1) {
			if (v1->isAdult != v2->isAdult) {
				Adult = 0;
			}
		}
		if (v1->runtime != -1 && v2->runtime != -1) {
			if (abs(v1->runtime - v2->runtime) < 20) {
				Runtime = 3;
			}
			else if (abs(v1->runtime - v2->runtime) < 40) {
				Runtime = 2;
			}
			else if (abs(v1->runtime - v2->runtime) < 60) {
				Runtime = 1;
			}
			else if (abs(v1->runtime - v2->runtime) >= 60) {
				Runtime = 0;
			}
		}
		
		vector<string> g1 = v1->genres;
		vector<string> g2 = v1->genres;
		vector<string> g(g1.size() + g2.size());
		vector<string>::iterator it;
		it = set_intersection(g1.begin(), g.end(), g2.begin(), g2.end(), g.begin());
		Genre = (double)(g.size()) * 10;
		SI = (Genre * Adult * Runtime * People) / (Seperations*4);
		return SI;
	}
	double calculateSID(Vertex* v1, Vertex* v2) {
		
		double Adult = 1;
		double Runtime = 3;
		double Genre = 1;
		double People = 1;
		double SI = 1;

		if (v1->title == v2->title) {
			return 0;
		}

		if (v1->isAdult != -1 && v2->isAdult != -1) {
			if (v1->isAdult != v2->isAdult) {
				Adult = 0;
			}
		}
		if (v1->runtime != -1 && v2->runtime != -1) {
			if (abs(v1->runtime - v2->runtime) < 20) {
				Runtime = 3;
			}
			else if (abs(v1->runtime - v2->runtime) < 40) {
				Runtime = 2;
			}
			else if (abs(v1->runtime - v2->runtime) < 60) {
				Runtime = 1;
			}
			else if (abs(v1->runtime - v2->runtime) >= 60) {
				Runtime = 0;
			}
		}
		string tid2 = movieNames[v2->title];
		if (v1->edges.find(tid2) != v1->edges.end()) {
			People = v1->edges[tid2].second;
		}

		vector<string> g1 = v1->genres;
		vector<string> g2 = v1->genres;
		vector<string> g(g1.size() + g2.size());
		vector<string>::iterator it;
		it = set_intersection(g1.begin(), g.end(), g2.begin(), g2.end(), g.begin());
		Genre = (double)(g.size()) * 15;

		SI = (Genre * Adult * Runtime * People);
		return SI;


	};


	vector<pair<string,double>> BreadthFirst(string input) {
		unordered_set<string> visited;
		string currentID = movieNames[input];
		Vertex* root = allMovies[currentID];
		Vertex* current = allMovies[currentID];

		queue<Vertex*> q;
		q.push(current);
		visited.insert(currentID);
		q.push(nullptr);
		int currentDepth = 1;

		priority_queue < pair<double, string>, vector<pair<double, string>>, compareFreq> pq;

		int count = 0;
		while (!q.empty()) {
			count++;
			if (count % 500 == 0) { cout << count << endl; }
			current = q.front();
			q.pop();
			if (current == nullptr) {
				currentDepth++;
				q.push(nullptr);
				if (q.front() == nullptr) { break; }
			}
			else {
				for (auto iter : current->edges) {
					string edgeID = iter.first;
					if (visited.find(iter.first) == visited.end()) {
						visited.insert(iter.first);
						q.push(iter.second.first);
						double currentSI = calculateSIB(root, iter.second.first, iter.second.second, currentDepth);
						pair<double, string> currPair(currentSI, iter.second.first->title);
						pq.push(currPair);
					}
				}
			}	
		}

		vector<pair<string,double>> returnVec;
		for (int i = 0; i < 10; i++) {
			if (pq.size() > 0) {
				pair<double, string> thing = pq.top();
				pq.pop();
				string title = thing.second;
				double SID = thing.first;
				pair<string, double> mov(title, SID);
				returnVec.push_back(mov);
			}
		}
		return returnVec;
	}

	vector<pair<string, double>> DepthFirst(string input) {
		unordered_set<string> visited;
		string currentID = movieNames[input];

		Vertex* current = allMovies[currentID];
		Vertex* root = allMovies[currentID];

		stack<Vertex*> s;
		s.push(current);
		visited.insert(currentID);

		priority_queue < pair<double, string>, vector<pair<double, string>>, compareFreq> pq;

		int count = 0;
		while (!s.empty()) {
			count++;
			if (count % 500 == 0) { cout << count << endl; }
			current = s.top();	

			for (auto iter : current->edges) {
				string edgeID = iter.first;
				if (visited.find(iter.first) == visited.end()) {
					visited.insert(iter.first);
					s.push(iter.second.first);
					
				}
			}

			Vertex* calcSI = s.top();
			double currentSI = calculateSID(root, calcSI);
			pair<double, string> currPair(currentSI, calcSI->title);
			pq.push(currPair);
			s.pop();
		}


		vector<pair<string, double>> returnVec;
		for (int i = 0; i < 10; i++) {
			if (pq.size() > 0) {
				pair<double, string> thing = pq.top();
				pq.pop();
				string title = thing.second;
				double SID = thing.first;
				pair<string, double> mov(title, SID);
				returnVec.push_back(mov);
			}
		}
		return returnVec;
	}
};



int main() {
	Graph filmFlicker(0);

	string filepath = "Small_Movies.tsv";
	filmFlicker.readfile(filepath);

	string filepath2 = "Small_process.tsv";
	filmFlicker.addEdge(filepath2);

	//filmFlicker.printVertices();

	string movieInput = "";

	ofstream MovieFile;
	MovieFile.open("MovieFile.txt");
	MovieFile.close();
	ofstream TimeFile;
	TimeFile.open("TimeFile.txt");
	TimeFile.close();

	while (true) {

		cout << "Input Movie: " << endl;
		getline(cin, movieInput);
		remove("MovieFile.txt");
		remove("TimeFile.txt");

		clock_t c_start = clock();
		vector<pair<string, double>> similarMoviesB = filmFlicker.BreadthFirst(movieInput);
		clock_t c_end = clock();

		MovieFile.open("MovieFile.txt");
		TimeFile.open("TimeFile.txt");
		TimeFile << (c_end - c_start) / CLOCKS_PER_SEC << endl;
		

		for (auto iter : similarMoviesB) {
			MovieFile << iter.first << endl;
		}

		clock_t c_start2 = clock();

		vector<pair<string, double>> similarMoviesD = filmFlicker.DepthFirst(movieInput);

		clock_t c_end2 = clock();

		TimeFile << (c_end2 - c_start2) / CLOCKS_PER_SEC << endl;

		TimeFile.close();
		MovieFile.close();



	}

	return 0;
};
