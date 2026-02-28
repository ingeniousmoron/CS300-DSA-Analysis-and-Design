//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Cielo Neal
// Version     : 1.0
// Copyright   : Copyright � 2023 SNHU COCE
// Description : Assignment 7-1 Project Two
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

struct Course {
	string courseNumber; // Unique identifier
	string courseName;
	vector<string> prereqList; // Vector to hold prerequisites for each course
};

struct Node {
	Course course;
	Node* left;
	Node* right;

	// default constructor
	Node() {
		left = nullptr;
		right = nullptr;
	}

	// initialize with a course
	Node(Course aCourse) :
		Node() {
		course = aCourse;
	}

	// destructor, allows destructor recurssion
	~Node() {
		delete left;
		delete right;
	}
};

//============================================================================
// Binary Search Tree class definition
//============================================================================

class BinarySearchTree {
private:
	Node* root;

	void addNode(Node* node, Course course);
	void inOrder(Node* node);
	Node* removeNode(Node* node, string courseNumber);
	
public:
	BinarySearchTree();
	virtual ~BinarySearchTree();
	void InOrder();
	void Insert(Course course);
	void Remove(string courseNumber);
	Course Search(string courseNumber);
};

// Default constructor
BinarySearchTree::BinarySearchTree() {
	root = nullptr;
}

// Destructor
BinarySearchTree::~BinarySearchTree() {
	// deleting the root will start deletion chain
	delete root;
}

// Traverse the tree in order
void BinarySearchTree::InOrder() {
	inOrder(root);
}


// Insert a course
void BinarySearchTree::Insert(Course course) {
	// if root is null course is new root
	if (root == nullptr) {
		root = new Node(course);
	} 
	else { // else call addNode
		this->addNode(root, course);
	}
}

// Remove a course
void BinarySearchTree::Remove(string courseNumber) {
	removeNode(root, courseNumber);
}

Course BinarySearchTree::Search(string courseNumber) {
	// set current node to the root
	Node* curNode = root;

	// keep looping downwards until bottom reached or matching courseNumber found
	while (curNode != nullptr) {
		if (curNode->course.courseNumber == courseNumber) {
			return curNode->course;
		} // traverse left if coursenumber is smaller than current node
		else if (courseNumber < curNode->course.courseNumber) {
			curNode = curNode->left;
		}
		else { // else larger so traverse right
			curNode = curNode->right;
		}
	}
	//return empty course if not found
	Course course;
	return course;
}

// add a course to some node
void BinarySearchTree::addNode(Node* node, Course course) {
	// create a new node for the course
	Node* newNode = new Node(course);

	// if node is larger than course add course left
	if (course.courseNumber < node->course.courseNumber) {
		// if no left node, course node is left
		if (node->left == nullptr) {
			node->left = newNode;
		}
		else { // otherwise recurse down the left node
			addNode(node->left, course);
		}
	}
	else {
		// if no right node, course node is right
		if (node->right == nullptr) {
			node->right = newNode;
		}
		else { // otherwise recurse down the right node
			addNode(node->right, course);
		}
	}
}

// print all courses in order
void BinarySearchTree::inOrder(Node* node) {
	// if node is not null
	if (node != nullptr) {
		inOrder(node->left);
		// print course information
		cout << node->course.courseNumber << ", " << node->course.courseName << endl;
		// if course has prerequisites
		if (node->course.prereqList.size() > 0) {
			cout << "Prerequisites: ";
			// print all prerequisites
			for (int i = 0; i < node->course.prereqList.size(); ++i) {
				cout << node->course.prereqList.at(i);
				if (i != node->course.prereqList.size() - 1) {
					cout << ", ";
				}
			}
			cout << endl;
		}
		inOrder(node->right);
	}
}

// Remove a bid from some node
Node* BinarySearchTree::removeNode(Node* node, string courseNumber) {
	// return node if null
	if (node == nullptr) {
		return node;
	}

	// (otherwise recurse down the left subtree)
	// check for match and if so, remove left node using recursive call
	if (courseNumber < node->course.courseNumber) {
		node->left = removeNode(node->left, courseNumber);
	}

	// (otherwise recurse down the right subtree)
	// check for match and if so, remove right node using recursive call
	else if (courseNumber > node->course.courseNumber) {
		node->right = removeNode(node->right, courseNumber);
	}

	else { // (otherwise no children so node is a leaf node)
		if (node->left == nullptr && node->right == nullptr) {
			delete node;
			node = nullptr;
		}
		// (otherwise check one child to the left)
		else if (node->left != nullptr && node->right == nullptr) {
			Node* tempNode = node;
			node = node->left;
			delete tempNode;
		}
		// (otherwise check one child to the right)
		else if (node->left == nullptr && node->right != nullptr) {
			Node* tempNode = node;
			node = node->right;
			delete tempNode;
		}
		// (otherwise more than one child so find the minimum)
		else {
			Node* tempNode = node->right;
			while (tempNode->left != nullptr) {
				tempNode = tempNode->left;
			}
			node->course = tempNode->course;
			removeNode(node->right, tempNode->course.courseNumber);
		}
	}
	
	return node;
}

//============================================================================
// Static functions
//============================================================================

// Display course information
void displayCourse(Course course) {
	cout << course.courseNumber << ", " << course.courseName << endl;
	// if course has prerequisites
	if (!course.prereqList.empty()) {
		cout << "Prerequisites: ";
		// print all prerequisites
		for (int i = 0; i < course.prereqList.size(); ++i) {
			cout << course.prereqList.at(i);
			if (course.prereqList.size() > 1 && i < course.prereqList.size() - 1) {
				cout << ", ";
			}
		}
		cout << endl;
	}
	return;
}

// Split each line into tokens
vector<string> splitLine(string lineFeed) {
	char delim = ',';
	lineFeed += delim; //includes a delimiter at the end so last word is also read
	vector<string> lineTokens;
	string temp = "";

	for (int i = 0; i < lineFeed.length(); i++)
	{
		if (lineFeed[i] == delim)
		{	// if a comma was accidentally stored, remove it
			if (temp == ",") {
				temp = "";
				i++;
			}
			else { //store words in token vector
				lineTokens.push_back(temp); 
				temp = ""; //reset temp
				i++;
			}			
		}
		temp += lineFeed[i];
	}
	return lineTokens;
}

// Load courses
void loadCourses(string csvPath, BinarySearchTree* courseList) {
	ifstream inFS; //instream for file
	string line;
	vector<string> stringTokens;

	inFS.open(csvPath);

	if (!inFS.is_open()) {
		cout << "Could not open file." << endl;
		return;
	}

	while (!inFS.eof()) {
		getline(inFS, line);
		stringTokens = splitLine(line);

		if (stringTokens.size() >= 2) {
			Course course; //create a new course for each line
			course.courseNumber = stringTokens.at(0);
			course.courseName = stringTokens.at(1);

			for (int i = 2; i < stringTokens.size(); i++) {
				course.prereqList.push_back(stringTokens.at(i));
			}
			courseList->Insert(course);
		}
	}
	inFS.close();
}

// convert the case of a string to upper
void changeCase(string& userString) {
	for (int i = 0; i < userString.length(); ++i) {
		if (isalpha(userString[i])) {
			userString[i] = toupper(userString[i]);
		}
	}
}


//============================================================================
// Main function
//============================================================================
int main(int argc, char* argv[]) {
	string csvPath, courseKey;

	switch (argc) {
	case 2:
		csvPath = argv[1];
		break;
	case 3:
		csvPath = argv[1];
		courseKey = argv[2];
		break;
	default:
		csvPath = "CS 300 ABCU_Advising_Program_Input.csv";
	}

	BinarySearchTree* courses;
	courses = new BinarySearchTree();
	Course course;

	cout << "Welcome to the course planner." << endl;
	int choice = 0;
	while (choice != 9) {
		cout << endl;
		cout << "1. Load Data Structure." << endl;
		cout << "2. Print Course List." << endl;
		cout << "3. Print Course." << endl;
		cout << "9. Exit" << endl << endl;
		cout << "What would you like to do? ";
		cin >> choice;

		switch (choice) {
		case 1:
			loadCourses(csvPath, courses);
			cout << "Courses loaded." << endl;
			break;

		case 2:
			courses->InOrder();
			break;
			
		case 3:

			cout << "What course do you want to know about? ";
			cin >> courseKey;

			changeCase(courseKey);

			course = courses->Search(courseKey);

			if (!course.courseNumber.empty()) {
				displayCourse(course);
			}
			else {
				cout << courseKey << " not found." << endl;
			}
			break;

		case 9:
			break;

		default:
			cout << choice << " is not a valid option." << endl << endl;
			cin.clear();
			cin.ignore();
		}
	}

	cout << "Thank you for using the course planner." << endl;

	return 0;
}