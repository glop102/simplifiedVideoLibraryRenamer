#include "glopConfig.h"

/*
The Magic Sauce

The method is recursive
find the key of the new parameter
match the ending char to the tokens that we have reserved
if we know what to do with it, then do the right action
if not, try to do the best action we can think of, print a message, and carry on

key  tokens - what we do when reaching certain tokens for the label
len>0  {   make a new group and recurse the parse function
len>0  =   parse for the value and assign the pair
len>0  #   error - read rest of the line
len>0  }   error - ignore just this character
len>0  \n  error - ignore just this character
len=0  }   closing group - simply unwind stack
len=0  \n  blank line - ignore just thist char
len=0  #   comment line - ignore line
len=0  =   error - read rest of the line
len=0  {   error - irnore just this character

value tokens
 \n  normal specification, just save the value pair
 }   normal specification and also closing the group it is in
 #   normal specification with a comment after it
 =   error - can't specify a value for a value
 {   error - cant open an unnamed group
*/

namespace GlopConfig{

char getKey(FILE* fd, std::string& key){
	key = "";
	int c = fgetc(fd);
	while(c!=EOF){
		if(c=='\t') // lets not deal with tabs
			c=' ';
		if(c=='=' || c=='{' || c=='}' || c=='\n' || c=='#')
			return c;
		else{
			if(c!=' ' || key.length()>0) // remove the preceding blank spaces, for indentation purposes
				key+=c; // nothing of note yet, keep going
			c = fgetc(fd);
		}
	}
	return c;
}

char getBody(FILE* fd,std::string& value){
	value = "";
	int c = fgetc(fd);
	while(c!=EOF){
		if(c=='\t') // lets not deal with tabs
			c=' ';
		if(c=='=' || c=='{' || c=='}' || c=='\n' || c=='#')
			return c;
		else{
			if(c!=' ' || value.length()>0) // remove the preceding blank spaces, for indentation purposes
				value+=c; // nothing of note yet, keep going
			c = fgetc(fd);
		}
	}
	return c;
}

void readRestOfLine(FILE* fd){
	int c=fgetc(fd);
	while(c!=EOF && c!='\n') c=fgetc(fd);
}
void removeTrailingSpaces(std::string& sss){
	int length = sss.length() - 1;
	if(length == 0) return;
	while(sss[length] == ' '){
		sss.erase(length,1); // erase the last char
		length --;
	}
}

void ParseFile_Recurse(FILE* fd, Settings& settings,int& lineCount){
	std::string key,value;
	char lastCharParsed;

	do{
		lastCharParsed = getKey(fd,key);
		removeTrailingSpaces(key);
		if(lastCharParsed == '=' && key.size()>0){
			lastCharParsed = getBody(fd,value);
			removeTrailingSpaces(value);
			settings.values[key] = value;
				//Now lets add a new nested tree of if statements
				//sorry future me about this mess
				if(lastCharParsed=='#'){
					readRestOfLine(fd);
					lineCount++;
				}else if(lastCharParsed=='}'){
					return; //unwind the stack to be done with this group
				}else if(lastCharParsed=='\n'){
					//nothing special
					lineCount++;
				}else if(lastCharParsed=='{'){
					printf("ERROR: CONFIG:%d: Opening new group ( { ) while specifing value\n",lineCount);
				}else if(lastCharParsed=='='){
					printf("ERROR: CONFIG:%d: Value specifing ( = ) while specifing value\n",lineCount);
				}else if( !feof(fd) ){
					printf("ERROR: CONFIG:%d: Reached EOF Unexpectedly (value parsing)\n",lineCount);
				}
		}else if(lastCharParsed == '{' && key.size()>0){
			//found a named group, so recurse down to get everything inside of it
			ParseFile_Recurse(fd,settings.groups[key],lineCount);
		}else if(lastCharParsed == '}' && key.size()==0){
			//found that we are closing the group we are currently in
			return; // unwind the stack
		}else if(lastCharParsed == '\n' && key.size()==0){
			//just ignore blank lines
			lineCount++;
		}else if(lastCharParsed == '#' && key.size()==0){
			//ignore lines that only have comments
			readRestOfLine(fd);
			lineCount++;
		}
		// end of good states, following are bad states
		else if( lastCharParsed=='#' && key.size()>0 ){
			readRestOfLine(fd);
			lineCount++;
			printf("ERROR: CONFIG:%d: found a comment token ( # ) while parsing label\n",lineCount);
		}else if( lastCharParsed=='=' && key.size()==0 ){
			readRestOfLine(fd);
			lineCount++;
			printf("ERROR: CONFIG:%d: found a ( = ) token with an empty label\n",lineCount);
		}else if( lastCharParsed=='}' && key.size()>0 ){
			printf("ERROR: CONFIG:%d: found a ( } ) token with a non-empty label\n",lineCount);
		}else if( lastCharParsed=='\n' && key.size()>0 ){
			printf("ERROR: CONFIG:%d: reached end of line with a non-empty label\n",lineCount);
			lineCount++;
		}else if( lastCharParsed=='{' && key.size()>0 ){
			printf("ERROR: CONFIG:%d: found a ( { ) token with an empty label\n",lineCount);
		}else if( feof(fd) ){
			if(key.size()>0)
				printf("ERROR: CONFIG:%d: Reached EOF Unexpectedly (key parsing)\n",lineCount);
			//else no-one cares as we are not losing any information
		}

	}while(!feof(fd));
}

Settings ParseFile(std::string filename){
	Settings settings;
	FILE* fd = fopen(filename.c_str(),"r");
	if(fd == NULL)
		return settings;

	int lineCount=1; // for getting accurate reporting of errors in the file
	ParseFile_Recurse(fd,settings,lineCount);

	fclose(fd);
	return settings;
}

void SaveToFile_Recurse(FILE* fd, Settings& s,std::string prefix){
	{ // write the current level key-value pairs first
		std::map<std::string,std::string>::iterator itt = s.values.begin();
		while(itt!=s.values.end()){
			fprintf(fd,"%s%s = %s\n",prefix.c_str(),itt->first.c_str(),itt->second.c_str());
			++itt;
		}
	}
	{ // recurse down into the groups
		std::map<std::string,Settings>::iterator itt = s.groups.begin();
		while(itt!=s.groups.end()){
			fprintf(fd,"%s%s {\n",prefix.c_str(),itt->first.c_str()); // header of the group
			SaveToFile_Recurse(fd,itt->second,prefix+"    ");         // all the data the group itself contains
			fprintf(fd, "%s}\n", prefix.c_str());                     // the closing bracket of the group
			++itt;
		}
	}
}

void SaveToFile(std::string filename,Settings& settings){
	FILE* fd = fopen(filename.c_str(),"w");
	if(fd == NULL)
		return; // couldn't open file

	//now recurse down the groups
	SaveToFile_Recurse(fd,settings,"");

	fclose(fd);
}

}; // namespace ConfigParser