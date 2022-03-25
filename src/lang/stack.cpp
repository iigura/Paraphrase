#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <boost/format.hpp>

#include "externals.h"
#include "typedValue.h"
#include "stack.h"

static void printLine(int inWidth);
static void printCenteringStr(std::string inString,int inWidth);

PP_API void ShowStack(Stack& inStack,const char *inStackName) {
	int maxLength=-1;
	std::string emptyMessageStr=(boost::format("(%s is empty)")%inStackName).str();
	if(inStack.size()<1) {
		maxLength=(int)emptyMessageStr.size()+2; // +2 for blanks(spaces) for both end.
	} else {
		const int nameLen=(int)strlen(inStackName);
		std::string spc(" ");
		for(int i=0; i<nameLen; i++) {
			spc+=" ";
		}
		std::string spc2("");
		for(int i=0; i<nameLen-3; i++) {
			spc+=" ";
		}
		std::vector<std::string> v;
		const size_t n=inStack.size();
		for(size_t i=0; i<n; i++) {
			std::string s;
			if(inStack[i].dataType==DataType::String) {
				s=inStack[i].GetTypeStr()+" '"+inStack[i].GetEscapedValueString(-1)+"'";
			} else {
				s=inStack[i].GetTypeStr()+" "+inStack[i].GetValueString(-1,false,true);
			}
			v.push_back(s);
			maxLength=std::max(maxLength,(int)s.size());
		}
		maxLength+=2;
		printf("%s",spc.c_str());
		fputs("   +",stdout); printLine(maxLength); fputs("+   \n",stdout);
		printf("%s",spc2.c_str()); fputs("TOS-->|",stdout);
			printCenteringStr(v[v.size()-1],maxLength);
		fputs("|   \n",stdout);
		for(int i=(int)v.size()-2; i>=0; i--) {
			printf("%s",spc.c_str()); fputs("   |",stdout);
				printCenteringStr(v[i],maxLength);
			fputs("|   \n",stdout);
		}
	}
	printf("%s:",inStackName);
	printLine(maxLength+8);	// +8 for '   |' ... '|   '
	putchar('\n');
	if(inStack.size()<1) {
		printf("    %s    \n",emptyMessageStr.c_str());
	}
}
static void printLine(int inWidth) {
	for(int i=0; i<inWidth; i++) {
		putchar('-');
	}
}
static void printCenteringStr(std::string inString,int inWidth) {
	int leftGap=(inWidth-(int)inString.size())/2;
	for(int i=0; i<leftGap; i++) {
		putchar(' ');
	}
	printf("%s",inString.c_str());
	int rightGap=inWidth-leftGap-(int)inString.size();
	for(int i=0; i<rightGap; i++) {
		putchar(' ');
	}
}

