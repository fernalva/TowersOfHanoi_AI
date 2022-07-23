#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
using namespace std;
// NAME: Fernando Alvarado
// Final Project: Towers of Hanoi using A*

class state
{
public:
  string format[3];  // made up of three strings going from LMS - - to - - LMS
  string camefrom[3];  // the parent on the solution path
  int g;  // number of disk moves so far
  int h;  // 3 - number of disks on goal peg
  int f;  // g + h
};

// major data structures
vector<state> frontier;
vector<state> beenThere;

// used to compare equality of arrays because it is done often throughout the program
// returns true if arrays are the same; returns false if they are not the same
bool isEqual(string a[3], string b[3])
{
  for(int i=0; i < 3; i++)
    {
      if(a[i] != b[i])
	return false;
    }
  return true;
}

// used to check if we have already been to this state before
// ensures that we do not end up in a loop visiting states that have already been visited
bool isInBeenThere(state x)
{
  for(int i=0; i < beenThere.size(); i++)
    {
      if(isEqual(x.format, beenThere[i].format))
      {
	cout << " been there already" << endl;
	return true;
      }      
    }

  return false;
}

// used to check whether or not a state we are trying to move to is legal or not
bool isIllegal(state x)
{
  for(int i=0; i<3; i++)
    {
      // if there is only one disk on each peg than no matter which disk is on which peg the state is legal
      // the problems come when there are multiple disks on one peg
      if(x.format[i].size() > 1)
	{
	  // if there is more than one disk on a peg the small disk should never be on the bottom
	  if(x.format[i][0] == 'S')
	    {
	      cout << "illegal!" << endl;
	      return true;
	    }
	  // the large disk should never be anywhere other than the bottom the stack in every legal state
	  if(x.format[i][1] == 'L' || x.format[i][2] == 'L')
	    {
	      cout << "illegal!" << endl;
	      return true;
	    }
	  // the last illegal state is if the small disk is sandwiched between
	  // the large diskon the bottom and the medium disk on the top
	  if(x.format[i][1] == 'S' && x.format[i][2] == 'M')
	    {
	      cout << "illegal!" << endl;
	      return true;
	    }
	}
    }//else it is legal
  cout << "legal!" << endl;
  return false;
}

// searches for a certain formation of disks in the beenThere vector
state searchBeenThere(string x[3])
{
  for(int i=0; i < beenThere.size(); i++)
    {
      if(isEqual(x,beenThere[i].format))
	return beenThere[i];
    }
  //if not in the beenThere vectore print an error
  cout << "ERROR: Parent Node not in beenThere vector" << endl;
  state empty;
  return empty;
}

// utility to add x to beenThere
void addtoBeenThere(state x)
{ beenThere.push_back(x); }

// utility to add x to frontier
void addtoFrontier(state x)
{ frontier.push_back(x); }

// utility to remove state x from frontier
void removefromFrontier(state x)
{
  vector<state> newfrontier;
  for(int k = 0; k < frontier.size(); k++)
    if(!(isEqual(x.format,frontier[k].format)))
      newfrontier.push_back(frontier[k]);
  
  frontier = newfrontier; 
}

// trace and display the solution path from goal to initial
void tracePath(state g)
{
  string initial[3] = {"LMS", "", ""};
  if(!(isEqual(g.format,initial)))
    {
      cout << "came from" << endl;
      cout << "[";
      for(int i=0; i < 3; i++)
	{
	  cout << "'" << g.camefrom[i] << "'";
	  if(i != 2)
	    cout << ", ";
	}
      cout << "] " << endl;

      tracePath(searchBeenThere(g.camefrom)); // recursively call function
    }
}

// Check to see if next's format is already in frontier and return true or false.
//   If it is already in frontier,
//   and if the next's f is better,
//   update the frontier node to next.  (called from generate)
bool inFrontier(state next)
{
  for(int i=0; i < frontier.size(); i++)
    {
      if(isEqual(next.format,frontier[i].format))
	{
	  cout << "Already in frontier" << endl;
	if(next.f < frontier[i].f)
	  {
	    cout << "f is better" << endl;
	    frontier[i] = next;
	  }
	else
	  cout << "f is not better" << endl;
	
	return true;
	}
    }
  cout << "Not in frontier yet" << endl;
  return false;
}

// Try to add next to frontier but stop if ['', '', 'LMS'] (called from generateAll)
void generate(state next)
{
  cout << "Trying to add: ";
  cout << "[";
  for(int i=0; i < 3; i++)
    {
      cout << "'" << next.format[i] << "'";
      if(i != 2)
	cout << ", ";
    }
  cout << "] ";

  string goal[3] = {"", "", "LMS"};
  if(isEqual(next.format,goal))  // the goal is reached
    {
      cout << endl << ">>Reached goal: ";
      cout << "[";
      for(int i=0; i < 3; i++)
	{
	  cout << "'" << next.format[i] << "'";
	  if(i != 2)
	    cout << ", ";
	}
      cout << "] " << endl;;

      tracePath(next); // display the solution path
      exit(0); //done
    }

  // if been there don't add to frontier
  if(isInBeenThere(next))
    return;
  // if illegal don't add to frontier
  if(isIllegal(next))
    return;

  //else compute h and f for next
  int h = 3 - next.format[2].size();
  next.h = h;
  next.f = next.g + next.h;

  if(!inFrontier(next))
    addtoFrontier(next); // add next to Frontier
}

// Generate all new states from current (called from main)
void generateAll(state current)
{
  state next;

  // each next is modified from current for ease
  current.g += 1; // update g to be used for next
  // storing the parent so that we can produce the solution path
  for(int i=0; i<3; i++)
    current.camefrom[i] = current.format[i];

  next = current; // starting point of next is current

  for(int i=0; i<3; i++)
    {
      char popped; // variable to hold the disk that we are popping from the string

      // if the peg is empty then just skip it
      if(current.format[i] != "")
	{
	  popped = next.format[i].back(); 	  // save the disk about to be moved
	  next.format[i].pop_back();	  // remove top disk
	  next.format[(i+1)%3] += popped;	  // move disk one peg over
	  generate(next);	  //generate this state
	  
	  // reset so we can do the same thing again but move the disk
	  // two pegs over instead of one
	  next = current;
	  
	  popped = next.format[i].back();
	  next.format[i].pop_back();
	  next.format[(i+2)%3] += popped;	  // move disk two pegs over
	  generate(next);	  // generate this state

	  // reset for the next loop
	  next = current;
	}
    }
}
// Find the best f state out of the frontier and return it (called from main)
state bestofFrontier()
{
  state best = frontier[0];
  for(int i=0; i < frontier.size(); i++)
    {
      if(frontier[i].f < best.f)
	best = frontier[i];
    }
  cout << "Best is: [";
  for(int i=0; i < 3; i++)
    {
      cout << "'" << best.format[i] << "'";
      if(i != 2)
	cout << ", ";
    }
  cout << "]" << endl;

  return best;
}

// Display the states in the frontier (called from main)
void displayFrontier()
{
  for(int k=0; k < frontier.size(); k++)
    {
      cout << "[";
      for(int i=0; i < 3; i++)
	{
	  cout << "'" << frontier[k].format[i] << "'";
	  if(i != 2)
	    cout << ", ";
	}
      cout << "]" << endl;

      cout << "g = " << frontier[k].g << " ";
      cout << "h = " << frontier[k].h << " ";
      cout << "f = " << frontier[k].f << endl;
    }
}

void framework()
{
  state current = {{"LMS" ,"", ""}, {"", "", ""}, 0, 3, 3}; // initial
  addtoFrontier(current);

  char ans;
  while(ans != 'n')
    {
      removefromFrontier(current);
      addtoBeenThere(current);

      cout << ">>Expand: ";
      cout << "[";
      for(int i=0; i < 3; i++)
        {
          cout << "'" << current.format[i] << "'";
          if(i != 2)
            cout << ", ";
        }
      cout << "]" << endl;
      generateAll(current); // new states are added to frontier

      cout << "Frontier is:" << endl;
      displayFrontier();
      current = bestofFrontier(); // pick the best state out of frontier
      cin >> ans;
    }
}

int main()
{
  framework();
  return 0;
}
