#ifndef SAVELOADER_H
#define SAVELOADER_H

void saveToFile(double leftXAlign, double leftYAlign, double rightXAlign, double rightYAlign, double leftRotationAngle, double rightRotationAngle);
void loadFromFile(double& leftXAlign, double& leftYAlign, double& rightXAlign, double& rightYAlign, double& leftRotationAngle, double& rightRotationAngle);

#endif // !SAVELOADER_H
