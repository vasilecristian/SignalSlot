//--------------------------------------------------------------------------------
// SignalSlot.hpp
//
// Copyright (c) 2022, Cristian Vasile
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL CRISTIAN VASILE BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: Cristian Vasile
//--------------------------------------------------------------------------------
#include <stdio.h>
#include <time.h>
#include <vector>
#include <iostream>

#include "SignalSlot/SignalSlot.hpp"


#define TEST_INIT(nameOfTest)	\
	std::string testName = nameOfTest;	\
	int testReturnValue = 0;

#define TEST_RETURN_VALUE testReturnValue

#define CHECK(condition) \
	if(!(condition)) { std::cout << "Failed: " << testName << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; testReturnValue++; }	\
	else { std::cout << "Passed: " << testName << std::endl; }

int Test1()
{
	TEST_INIT("Test1!");
	
	// Arrange
	Signal<int> signal;
	int value = 0;
	int valueExpected = 42;
	signal.Connect([&value](int v) { value = v;});
	
	// Act
	signal.Emit(42);
	
	// Assert
	CHECK(value == valueExpected);
	
	
	return TEST_RETURN_VALUE;
}

int Test2()
{
	TEST_INIT("Test2: DisconnectAll!");
	
	// Arrange
	Signal<int> signal;
	int value = 0;
	int valueExpected = 0;
	signal.Connect([&value](int v) { value = v;});
	signal.Connect([&value](int v) { value = v;});
	signal.Connect([&value](int v) { value = v;});
	signal.Connect([&value](int v) { value = v;});
	
	// Act
	signal.DisconnectAll();
	signal.Emit(42);
	
	// Assert
	CHECK(value == valueExpected);
	
	
	return TEST_RETURN_VALUE;
}

int Test3()
{
	TEST_INIT("Test3: Disconnect only one slot!");
	
	// Arrange
	Signal<int> signal;
	int value = 0;
	int valueExpected = 42;
	int slot1Id = signal.Connect([&value, &valueExpected](int) { value = 1;});
	int slot2Id = signal.Connect([&value, &valueExpected](int) { value = valueExpected;});
	
	// Act
	signal.Disconnect(slot1Id);
	signal.Emit(0);
	
	// Assert
	CHECK(value == valueExpected);
	
	
	return TEST_RETURN_VALUE;
}

int Test4()
{
	TEST_INIT("Test4: Emit only for a slot!");
	
	// Arrange
	Signal<int> signal;
	int value = 0;
	int valueExpected = 42;
	int slot1Id = signal.Connect([&value, &valueExpected](int) { value = 1;});
	int slot2Id = signal.Connect([&value, &valueExpected](int) { value = valueExpected;});
	
	// Act
	signal.EmitFor(slot2Id, 0);
	
	// Assert
	CHECK(value == valueExpected);
	
	
	return TEST_RETURN_VALUE;
}

int Test5()
{
	TEST_INIT("Test5: Emit for all except a slot!");
	
	// Arrange
	Signal<int> signal;
	int value = 0;
	int valueExpected = 42;
	int slot1Id = signal.Connect([&value, &valueExpected](int) { value = 1;});
	int slot2Id = signal.Connect([&value, &valueExpected](int) { value = valueExpected;});
	
	// Act
	signal.EmitForAllButOne(slot1Id, 0);
	
	// Assert
	CHECK(value == valueExpected);
	
	
	return TEST_RETURN_VALUE;
}


int main(int argc, char* argv[])
{
	int returnValue = 0;
	
	returnValue += Test1();
	returnValue += Test2();
	returnValue += Test3();
	returnValue += Test4();
	returnValue += Test5();

    return returnValue;
}



