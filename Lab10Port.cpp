//============================================================================
// Name        : Lab10Port.cpp
// Author      : Ian Naeger
// Version     :
// Copyright   : N/A
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <cstring>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>


using namespace std;

class message {
	protected:
		string msg;
		int length;

	public:
		message( void );
		message( string );
		~message( void );
		void printInfo( void );
};
// message class, is base for morse code class

class morseCodeMessage : public message{
	public:
		string *TrnsMsg;
		string mcm;
		int index;
		morseCodeMessage(void);
		~morseCodeMessage(void);
		morseCodeMessage( string );
		void translate(void);
		void printInfo(void);
		void MorseCodeToLights(void);
};
// morse code class

message::message(void)
{
	cout<<"Enter a word:"<<endl;
	cin>>msg;
	length = msg.length();
}// default constructor

message::message( string input )
{
	msg = input;
	length = msg.length();
}// constructor with and input

message::~message(void)
{
	//Nothing to deallocate
}// destructor with nothing, because no memory needs to be cleared

void message::printInfo(void)
{
	cout<<"Normal Word: "<<msg<<endl;
}// prints the og message

morseCodeMessage::morseCodeMessage(void) : message()
{
	TrnsMsg = new string[ length ];
	cout<<"Test"<<endl;
	translate( );
}// defaultly constructs morse code, and calls translate

morseCodeMessage::morseCodeMessage( string input ) : message( input )
{
	cout<<"Testinput"<<endl;
	TrnsMsg = new string[ length ];
	translate( );
}// constructs morse code with input, and calls translate

morseCodeMessage::~morseCodeMessage( void )
{
	delete[] TrnsMsg;

}// destructor for morse code


void morseCodeMessage::printInfo(void)
{
	cout<<"Normal Word: "<<msg<<endl;
	cout<<"Morse Code: "<<mcm<<endl;
}// prints both normal message and morse code

void morseCodeMessage::translate(void)
{
	int i, j, k, check;
	string spacing = " ";
	string null = "\0";
	string letters = "abcdefghijklmnopqrstuvwxyz123456789 ";
	string morse[37] =  {
	".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--","-.","---",
	".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--..",    ".----","..---",
	"...--","....-",".....","-....","--...","---..","----.","-----","/" };

	for( k=0; k < length; k++)
	{
		check = isupper( msg[k] );
		if( check == 1 )
		{
			msg[k] = (char)tolower(msg[k]);
		}
	}

//--------------------------
	for( i=0; i < length; i++)
	{
		index = letters.find( msg[i] );
		TrnsMsg[i] = morse[index];
	}

	for( j=0; j < length; j++)
	{
		mcm.append(TrnsMsg[j]);
		mcm.append(spacing);
	}
	mcm.append(null);

//------------------------------

}// changes everything to lower case, finds the letter in morse array, then adds morse code to arrray

void morseCodeMessage::MorseCodeToLights(void)
{
	int fd;		// for the file descriptor of the special file we need to open.
	unsigned long *BasePtr;		// base pointer, for the beginning of the memory page (mmap)
	unsigned long *PBDR, *PBDDR;	// pointers for port B DR/DDR

	fd = open("/dev/mem", O_RDWR|O_SYNC);	// open the special file /dem/mem
	if(fd == -1){
	printf("\n error\n");
	}

		// We need to map Address 0x80840000 (beginning of the page)
	BasePtr = (unsigned long*)mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0x80840000);
	if(BasePtr == MAP_FAILED){
	   	printf("\n Unable to map memory space \n");
	}  // failed mmap

		// To access other registers in the page, we need to offset the base pointer to reach the
		// corresponding addresses. Those can be found in the board's manual.
	PBDR = BasePtr + 1;		// Address of port B DR is 0x80840004
	PBDDR = BasePtr + 5;	// Address of port B DDR is 0x80840014

//	*PBDDR |= 0x80;			// configures port B7 as output (Green LED)
//   *PBDDR |= 0x40;			// configures port B6 as output (Yellow LED)
//   *PBDDR |= 0x20;			// configures port B5 as output (Red LED)
	*PBDDR |= 0xE0;
	*PBDDR &= 0xFFFFFFFE;	// configures port B0 as input (first push button). You could use: &= ~0x01

	int k;
	char mes;
	for(k=0; k<mcm.length(); k++)
	{
		mes = mcm[k];

		if(mes == '.')
		{
		    *PBDR |= 0x20;	// ON: write a 1 to port B0. Mask all other bits.
			sleep(1);
		    *PBDR &= ~0x20;
		}
		else if(mes == '-')
		{
		    *PBDR |= 0x40;	// ON: write a 1 to port B0. Mask all other bits.
			sleep(1);
		    *PBDR &= ~0x40;
		}
		else if(mes == ' ')
		{
			sleep(3);
		}
		else if(mes == '/')
		{
		    *PBDR |= 0x80;	// ON: write a 1 to port B0. Mask all other bits.
			sleep(1);
		    *PBDR &= ~0x80;
		}
		sleep(1);
	}
	*PBDR |= 0x80;	// ON: write a 1 to port B0. Mask all other bits.
	sleep(1);
	*PBDR &= ~0x80;
// "Prints" the Morse Code Message in LED lights on the board
}

int main(void)
{
	message msg1;
	message msg2("Two");
	morseCodeMessage msg3;


	cout<<"Testsadfdfdsafdsaf"<<endl;



	cout<<endl<<"Printing Message 1..."<<endl;
	msg1.printInfo();

	cout<<endl<<"Printing Message 2..."<<endl;
	msg2.printInfo();

	cout<<endl<<"Printing Message 3..."<<endl;
	msg3.printInfo();
	msg3.MorseCodeToLights();

// Demonstrates stack portion of the lab
	return 0;

}
