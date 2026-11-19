#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#define FILE_NAME "stereo_align.txt"

void saveToFile(double leftXAlign, double leftYAlign, double rightXAlign, double rightYAlign, double leftRotationAngle, double rightRotationAngle	){
	ofstream outFile(FILE_NAME);
	if (outFile.is_open()) {
		// Write data to the file
		outFile << leftXAlign << " " << leftYAlign << " " << rightXAlign << " " << rightYAlign << " " << leftRotationAngle << " " << rightRotationAngle << endl;
		outFile.close();
	} else {
		cerr << "Unable to open file for writing: " << FILE_NAME << endl;
	}	
}

void loadFromFile(double &leftXAlign, double &leftYAlign, double &rightXAlign, double &rightYAlign, double &leftRotationAngle, double &rightRotationAngle	){
	ifstream inFile(FILE_NAME);
	if (inFile.is_open()) {
		// Read data from the file
		inFile >> leftXAlign >> leftYAlign >> rightXAlign >> rightYAlign >> leftRotationAngle >> rightRotationAngle;
		cout << "Loaded values: " << leftXAlign << ", " << leftYAlign << ", " << rightXAlign << ", " << rightYAlign << ", " << leftRotationAngle << ", " << rightRotationAngle << endl;
		inFile.close();
	} else {
		cerr << "Unable to open file for reading: " << FILE_NAME << endl;
	}
}