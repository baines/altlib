#define ALT_STR_IOSTREAM 1
#include "../alt_str.h"
#include <cstdio>
#include <cassert>
#include <iostream>

#define TEST_EQ(obj, code, expect) ({ \
	printf("Testing [ %-55s]... ", #obj " " #code); \
	obj code; \
	if(obj == expect){ \
		std::cout << "Ok." << std::endl; \
	} else { \
		std::cout << "Error!\n\tExpected '" << expect << "' Got '" << obj << "'" << std::endl; \
		exit(1); \
	} \
})

using namespace alt;

int main(void){

	StrMut str = "Hello.";

	TEST_EQ(str,, "Hello.");
	TEST_EQ(str, .append(" World."), "Hello. World.");
	
	char* p;
	TEST_EQ(p, = std::find(str.begin(), str.end(), '.'), str.data() + 5);
	
	TEST_EQ(str,
		.replace(p, p + 3, ", w"),
		"Hello, world."
	);
	
	TEST_EQ(str, 
		.insert(5, " there"),
		"Hello there, world."
	);
	
	TEST_EQ(str,
		.erase(1, 11),
		"H world."
	);
	TEST_EQ(str,
		.insert(1, "i"),
		"Hi world."
	);
	TEST_EQ(str,
		.append({ " Nice ", "to", " meet ", "you!" }),
		"Hi world. Nice to meet you!"
	);
	TEST_EQ(str,
		.append(4, "!"),
		"Hi world. Nice to meet you!!!!!"
	);
	TEST_EQ(str,
		.replace(str.begin() + 10, str.end(), "What's up?"),
		"Hi world. What's up?"
	);
	TEST_EQ(str,
		.replace(3, 5, "friend"), 
		"Hi friend. What's up?"
	);
	
	size_t c;
	
	TEST_EQ(str,
		= "this is a test",
		"this is a test"
	);
	
	TEST_EQ(c, = str.find ('i'),      2);
	TEST_EQ(c, = str.rfind('i'),      5);
	TEST_EQ(c, = str.find ("is", 3),  5);
	TEST_EQ(c, = str.rfind("is", 4),  2);
	TEST_EQ(c, = str.find_any ("as"), 3);
	TEST_EQ(c, = str.rfind_any("as"), 12);
	TEST_EQ(c, = str.find('z'),       str.size());
	
	puts("All Tests passed.");
	
	return 0;
}
