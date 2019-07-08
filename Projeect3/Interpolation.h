#pragma once
#include "stdafx.h"
#include<stdio.h>
#include<conio.h>
#include<math.h>
#define n_array 32
struct Interpolation{
private:
	double x[n_array],y[n_array];
public:
	double result(double var);
};