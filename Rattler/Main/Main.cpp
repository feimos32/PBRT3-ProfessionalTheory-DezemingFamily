#include <iostream>
#include "ModelLoad.h"
#include "ImageProcess.h"
#include <string>

int main(void) {


	Rattler::ModelLoad model;
	model.loadModel("./Resources/Girl/091_W_Aya_30K.obj", "./Resources/Output");

	return 0;
}











