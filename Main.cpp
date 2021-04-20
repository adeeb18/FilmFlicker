/*
COP3530 Final Project
Using imdb dataset to create a movie similarity prediction app
Authors: Adeeb Rashid, Caroline Rogers, Zachary Speaks
Last Edited: 4/19/2021
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

//Vertex Class
class Vertex {

public:
	//Members were left public to experiment with serialization

	//Movie Attributes
	string title;
	int isAdult;
	int year;
	int runtime;
	vector<string> genres;

	//Adjacency List
	unordered_map<string,pair<Vertex*, double>> edges;
	
	//Vertex Constructors
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

//Priority Queue max condition override
struct compareFreq {
	bool operator()(pair<double, string> n1, pair<double, string> n2) {
		//Compare stringency indices
		double f1 = n1.first;
		double f2 = n2.first;
		return f1 < f2;
	}
};

//Graph Class
class Graph {

public:
	//Map of tid to movie Vertex
	unordered_map<string, Vertex*> allMovies;
	//Map of movie name to tid
	unordered_map<string, string> movieNames;
	//Movie Counter
	int numVertices = 0;

	//Graph constructors
	Graph() {
		this->numVertices = 0;
	}
	Graph(int num) { this->numVertices = num; }

	//Add Vertex to allMovies map when reading file
	void addVertex(string _tid, Vertex* _vert) {
		this->allMovies.insert({ _tid, _vert });
		this->numVertices += 1;
	}
	
	//Reading file to create allMovies map
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

		//Reading all lines of file
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

			//Splitting genre string and pushing to vector
			while (genres.find(",") != -1) {
				string delim = ",";
				currentGenre = genres.substr(0, genres.find(delim));
				genres = genres.substr(genres.find(delim) + 1, genres.length() - genres.find(delim) + 1);
				_genres.push_back(currentGenre);
			}
			currentGenre = genres;
			_genres.push_back(currentGenre);

			//Vertex creation
			Vertex* movie = new Vertex(_title, _isAdult, _year, _runtime, _genres);

			//Adding vertex to graph 
			this->addVertex(_tid, movie);
			movieNames[_title] = _tid;
		}
	}
}

	//Adding edges with people list
	void addEdge(string filepath) {
	ifstream file(filepath);
	int count = 0;
	if (file.is_open()) {
		string line;
		string trash;
		string delimiter = "\t";
		string delim = ",";
		string movie;

		//Iterate through every line in file
		while (getline(file, line)) {
			count++;
			if (count % 500 == 0) { cout << count << endl; }
			//A:trash
			trash = line.substr(0, line.find(delimiter));
			//List of movies
			line = line.substr(line.find(delimiter) + 1, line.length() - line.find(delimiter) + 1);
			//Get rid of brackets
			line = line.substr(1, line.size() - 2);
			vector<string> career;

			//Create career vector of every movie one person is associated with
			while (line.find(",") != -1) {
				movie = line.substr(1, line.find(delim) - 2);
				line = line.substr(line.find(delim) + 2, line.length() - line.find(delim) + 1);
				career.push_back(movie);
			}
			line = line.substr(1, line.size() - 2);
			career.push_back(line);

			//Create edges using all combinations of movies in career vector
			for (int i = 0; i < career.size(); i++) {
				for (int j = i + 1; j < career.size(); j++) {
					if (allMovies.find(career[i]) != allMovies.end()) {
						if (allMovies.find(career[j]) != allMovies.end()) {
							//If edge is not already present, create bidirectional edge
							if (allMovies[career[i]]->edges.find(career[j]) == allMovies[career[i]]->edges.end()) {
								pair<Vertex*, double> newEdge(allMovies[career[j]],1);
								allMovies[career[i]]->edges[career[j]] = newEdge;

								pair<Vertex*, double> newEdge2(allMovies[career[i]], 1);
								allMovies[career[j]]->edges[career[i]] = newEdge2;
							}
							//If edge is already present, increase weight
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

	//Print vertices to check all movies have been loaded
	void printVertices() {
		for (auto iter : allMovies) {
			//Printing single valued attributes
			cout << "tid: " << iter.first << " title: " << iter.second->title << " isAdult: " << iter.second->isAdult << " year: " << iter.second->year << " runtime: " << iter.second->runtime << endl;
			//Printing genre list
			cout << "genres: " << endl;
			for (auto it : iter.second->genres) {
				cout << it << endl;
			}
			//Printing edge list with weights
			for (auto it : iter.second->edges) {
				cout << it.first << " " << it.second.second << endl;
			}
			cout << endl;
		}
	}

	//Calculating Stringency Index for Breadth First Search
	double calculateSIB(Vertex* v1, Vertex* v2, double edgeWeight, int depth) {

		double Adult = 1;
		double Runtime = 3;
		double Genre = 1;
		double People = edgeWeight;
		double Seperations = depth;
		double SI = 1;

		//Adult movies can only be matched with other ones and vice versa
		if (v1->isAdult != -1 && v2->isAdult != -1) {
			if (v1->isAdult != v2->isAdult) {
				Adult = 0;
			}
		}
		//Runtime similarity criteria
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
		
		//Determining number of similarities in genres
		vector<string> g1 = v1->genres;
		vector<string> g2 = v1->genres;
		vector<string> g(g1.size() + g2.size());
		vector<string>::iterator it;
		it = set_intersection(g1.begin(), g.end(), g2.begin(), g2.end(), g.begin());
		Genre = (double)(g.size()) * 10;

		//Calculating Similarity Index
		SI = (Genre * Adult * Runtime * People) / (Seperations*4);
		return SI;
	}

	//Calculating Stringency Index for Depth First Search
	double calculateSID(Vertex* v1, Vertex* v2) {
		
		double Adult = 1;
		double Runtime = 3;
		double Genre = 1;
		double People = 1;
		double SI = 1;

		//Do not return the same movie
		if (v1->title == v2->title) {
			return 0;
		}

		//Adult movies can only be matched with other ones and vice versa
		if (v1->isAdult != -1 && v2->isAdult != -1) {
			if (v1->isAdult != v2->isAdult) {
				Adult = 0;
			}
		}

		//Runtime similarity criteria
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

		//Determining number of similarities in genres
		vector<string> g1 = v1->genres;
		vector<string> g2 = v1->genres;
		vector<string> g(g1.size() + g2.size());
		vector<string>::iterator it;
		it = set_intersection(g1.begin(), g.end(), g2.begin(), g2.end(), g.begin());
		Genre = (double)(g.size()) * 15;

		//Calculating Similarity Index
		SI = (Genre * Adult * Runtime * People);
		return SI;
	};

	//Breadth First Search for Similar Movies
	vector<pair<string,double>> BreadthFirst(string input) {
		//Storing visited Vertices
		unordered_set<string> visited;

		string currentID = movieNames[input];
		Vertex* root = allMovies[currentID];
		Vertex* current = allMovies[currentID];

		//Organizing breadth first search with a queue
		queue<Vertex*> q;
		q.push(current);
		visited.insert(currentID);
		q.push(nullptr);
		int currentDepth = 1;

		//List of vertices stored by max similarity index
		priority_queue < pair<double, string>, vector<pair<double, string>>, compareFreq> pq;

		int count = 0;

		//Iterating through graph
		while (!q.empty()) {
			count++;
			if (count % 500 == 0) { cout << count << endl; }
			current = q.front();
			q.pop();

			//Showing new depth level
			if (current == nullptr) {
				currentDepth++;
				q.push(nullptr);
				if (q.front() == nullptr) { break; }
			}

			//Calculating similarity index for each vertex
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

		//Returning top ten similar movies
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

	//Depth First Search for Similar Movies
	vector<pair<string, double>> DepthFirst(string input) {
		//Set to store visited Vertices
		unordered_set<string> visited;
		string currentID = movieNames[input];

		Vertex* current = allMovies[currentID];
		Vertex* root = allMovies[currentID];

		//Organizing search with a stack
		stack<Vertex*> s;
		s.push(current);
		visited.insert(currentID);

		//List of vertices stored by max similarity index
		priority_queue < pair<double, string>, vector<pair<double, string>>, compareFreq> pq;

		int count = 0;

		//Iterating through graph
		while (!s.empty()) {
			count++;
			if (count % 500 == 0) { cout << count << endl; }
			current = s.top();	

			//Pushing to stack
			for (auto iter : current->edges) {
				string edgeID = iter.first;
				if (visited.find(iter.first) == visited.end()) {
					visited.insert(iter.first);
					s.push(iter.second.first);
					
				}
			}

			//Calculating Similarity Index
			Vertex* calcSI = s.top();
			double currentSI = calculateSID(root, calcSI);
			pair<double, string> currPair(currentSI, calcSI->title);
			pq.push(currPair);
			s.pop();
		}

		//Returning top ten most similar movies
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

	//Create Graph
	Graph filmFlicker(0);

	//Load in all Vertices
	string filepath = "Final_Movies.tsv";
	filmFlicker.readfile(filepath);

	//Load in all Edges
	string filepath2 = "process.tsv";
	filmFlicker.addEdge(filepath2);

	//Check for all vertices and edges in graph
	//filmFlicker.printVertices();

	//Print output of time and movie list to file
	string movieInput = "";
	ofstream MovieFile;
	MovieFile.open("MovieFile.txt");
	MovieFile.close();
	ofstream TimeFile;
	TimeFile.open("TimeFile.txt");
	TimeFile.close();

	//Ask for movie input to return similar movies
	cout << "Input Movie: " << endl;
	getline(cin, movieInput);
	remove("MovieFile.txt");
	remove("TimeFile.txt");

	//Time Breadth First Search
	clock_t c_start = clock();
	vector<pair<string, double>> similarMoviesB = filmFlicker.BreadthFirst(movieInput);
	clock_t c_end = clock();

	//Print Results
	MovieFile.open("MovieFile.txt");
	TimeFile.open("TimeFile.txt");
	TimeFile << (c_end - c_start) / CLOCKS_PER_SEC << endl;
		
	for (auto iter : similarMoviesB) {
		MovieFile << iter.first << endl;
	}

	//Time Depth First Search
	clock_t c_start2 = clock();
	vector<pair<string, double>> similarMoviesD = filmFlicker.DepthFirst(movieInput);
	clock_t c_end2 = clock();

	//Return results
	TimeFile << (c_end2 - c_start2) / CLOCKS_PER_SEC << endl;

	//Close files
	TimeFile.close();
	MovieFile.close();

	return 0;
};
