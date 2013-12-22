/*
PROGRAM FOR CONSTRUCTION OF SLR PARSING TABLE FOR FOLLOWING GRAMMER
E->E+T
E->T
T->T*F
T->F
F->(E)
F->id
MADE BY--- JAS KANWALDEEP
DATE------ 29 November,2008
*/

#include<iostream.h>
#include<conio.h>
#include<iomanip.h>
#include<process.h>
#include<string.h>

#define MAX 30 
#define terminal(X) (((X)>=4 && (X)<=9)?1:0)

const int S1=0,E=1,T=2,F=3,PS=4,MP=5,LP=6,RP=7,id=8,DOLLAR=9;
const int NO_P=7,NO_S=9,NO_NT=4,END=99,err=100,NOTFOUND=100;
enum parser_moves{ERROR=101,ACCEPT,REDUCE,SHIFT,NON_TERMINAL};
struct production
{
	int number;
	int length;
	int left;
	int right[20];
};
struct action
{
	parser_moves move;
	int number;
};

//function prototypes
void print_productions();
void item_generate(int *n,int [][21]);
void print_item(int *);
int check(int [][21],int,int,int);
void print_items(int [][21],int);
void create_first();
void print_first();
void create_follow();
void print_follow();
void create_table(int [][21],int);
void print_table(int);
void check_input();
//global variables

char *symbols[]={"S","E","T","F","+","*","(",")","id","$"};
production grammer[10]={{0,1,S1,{E,END}},{1,3,E,{E,PS,T,END}},{2,1,E,{T,END}},{3,3,T,{T,MP,F,END}},{4,1,T,{F,END}},{5,3,F,{LP,E,RP,END}},{6,1,F,{id,END}}};
int goto_v[20][21];
int goto_d[20][21];
//First() AND Follow() SETS
int first[NO_S][NO_S-NO_NT];
int first_d[NO_S]={0};
int follow[NO_NT][10]={{0},{0}};
int follow_d[NO_NT]={0,0};
action slr_table[MAX][MAX];
int stck[MAX],top=-1;

int main()
{	
	int no_of_items=0;
	int item_set[20][21];//item[][]={{production_number,pointer,..}}
	cout<<"The Productions Are\n";
	print_productions();
	item_generate(&no_of_items,item_set);
	cout<<"No Of ITEM SETS: "<<no_of_items+1<<endl<<endl;
	print_items(item_set,no_of_items);
	create_first();
	print_first();
	create_follow();	
	print_follow();
	create_table(item_set,no_of_items);
	print_table(no_of_items);
	check_input();
	return 0;
}

void print_productions()
{
	for(int i=0;i<NO_P;i++)
	{
		cout<<symbols[grammer[i].left]<<"->";
		for(int j=0;grammer[i].right[j]!=END;j++)
		{
			cout<<symbols[grammer[i].right[j]];
		}
		cout<<endl;
	}
	cout<<endl;
}

void item_generate(int *n,int item_set[][21])
{
	void closure(int [][21],const int,int);
	int goto_f(int [][21],int* const,int,int);
	item_set[0][0]=0;
	item_set[0][1]=0;
	int done=0;
	int i=0,item_n=0,pointer=0,end=1,flag=1;
	do
	{
		if(flag==1)
			closure(item_set,i,end);
		flag=0;
		//cout<<"I"<<i<<endl;
		//print_item(&item_set[i][0]);
		//cout<<endl;
		do
		{
			if(!goto_d[item_n][pointer/2])
			{
				end=goto_f(item_set,&i,item_n,pointer);
				flag=1;
			}
			pointer+=2;
			if(item_set[item_n][pointer]==END)
			{
				pointer=0;
				item_n++;
				if(item_n>i)
					done=1;
			}
		}while(end==err && !done);
	}while(!done);
	*n=i;
}//end of items_generate()

void closure(int item_set[][21],const int i,int end)
{
	
	int curr=0;
	int added[][2]={{S,1},{E,0},{T,0},{F,0}};
	while(curr<end)
	{
		int j=item_set[i][curr];
		if(!terminal(grammer[j].right[item_set[i][curr+1]])) //check if in A->a.b b is a non terminal
		{
			int non_term=grammer[j].right[item_set[i][curr+1]];
			if(added[non_term][1]==0)
			{
				added[non_term][1]=1;
				j=0;
				while(grammer[j].left!=non_term && j<NO_P)
					j++;
				while(grammer[j].left==non_term)
				{
					item_set[i][++end]=grammer[j].number;
					item_set[i][++end]=0;
					j++;
				}
			}
		}
		curr+=2;
	}//end of outer while
	item_set[i][++end]=END;
}//end of closure()

int goto_f(int item_set[][21],int * const i,int item_n,int pointer)
{
	int j=item_set[item_n][pointer];
	int dot_pos=item_set[item_n][pointer+1];
	int num;
	num=grammer[j].number;
	int sym=grammer[j].right[dot_pos];
	int cvalue=check(item_set,num,dot_pos+1,*i);
	if(sym!=END && cvalue==NOTFOUND)
	{
		(*i)++;
		int curr=0,t=0;
		j=0;
		item_set[*i][curr]=num;
		item_set[*i][++curr]=dot_pos+1;
		goto_v[item_n][pointer/2]=*i;
		goto_d[item_n][pointer/2]=1;
		while(item_set[item_n][t]!=END)
		{
			j=item_set[item_n][t];	
			if(j!=num)
			{
				dot_pos=item_set[item_n][t+1];
				if(grammer[j].right[dot_pos]==sym)
				{
					item_set[*i][++curr]=j;
					item_set[*i][++curr]=dot_pos+1;
					goto_v[item_n][t/2]=*i;
					goto_d[item_n][t/2]=1;
				}
			}
			t+=2;
		}
		return curr;
	}
	else
	{
		if(sym==END)
		{
			goto_v[item_n][pointer/2]=item_n;
			goto_d[item_n][pointer/2]=1;
		}
		else
		{
			goto_v[item_n][pointer/2]=cvalue;
			goto_d[item_n][pointer/2]=1;

		}
		return err;
	}
}//end of goto_f()

void print_item(int *item_set)
{
	int l=0,flag;
	while(*(item_set+l)!=END)
	{
		flag=0;
		int r=0;
		while(grammer[r].number != *(item_set+l))
			r++;
		cout<<symbols[grammer[r].left]<<"->";
		for(int q=0;grammer[r].right[q]!=END;q++)
		{
			if(q==*(item_set+l+1))
			{
				flag=1;
				cout<<".";
			}
			cout<<symbols[grammer[r].right[q]];
		}
		if(flag==0)
			cout<<".";
	    cout<<endl;
		l+=2;
	}
}


int check(int item_set[][21],int num,int dot_pos,int index)
{
	int i,j;
	for(i=0;i<=index;i++)
	{
		for(j=0;item_set[i][j]!=END;j+=2)
		{
			if(item_set[i][j]==num)
			{
				if(item_set[i][j+1]==dot_pos)
					return i;
			}
		}
	}
	return NOTFOUND;
}

void print_items(int item_set[][21],int n)
{
	int i,j,flag;
	for(i=0;i<=n;i++)
	{
		flag=0;
		cout<<"I["<<i<<"]"<<endl;
		for(j=0;item_set[i][j]!=END;j+=2)
		{
			int r=item_set[i][j];
			cout<<symbols[grammer[r].left]<<"->";
			for(int q=0;grammer[r].right[q]!=END;q++)
			{
				if(q==item_set[i][j+1])
				{
					flag=1;
					cout<<".";
				}
				cout<<symbols[grammer[r].right[q]];
			}
			if(flag==0)
				cout<<".";
			//cout<<"\tIn"<<goto_v[i][j/2];
			cout<<endl;
		}
		cout<<endl;
	}
}

void create_first()
{
	void find_first(int);
	for(int i=NO_S;i>=0;i--)
	{
		if(!first_d[i])
			find_first(i);
	}
}

void find_first(int X)
{
	int i=-1;
	if(terminal(X))
	{
		first[X][++i]=X;
		first[X][++i]=END;
		first_d[X]=1;
	}
	else //non-terminal
	{
		for(int j=0;j<NO_P;j++)
		{
			if(grammer[j].left==X)
			{
				int rs=grammer[j].right[0];//Assuming No NULL Productions
				if(X!=rs)
				{
					if(!first_d[rs])
						find_first(rs);
					for(int k=0;first[rs][k]!=END;k++)
						first[X][++i]=first[rs][k];
				}
				
			}
		}
		first[X][++i]=END;
		first_d[X]=1;
	}
}

void print_first()
{
	int i,j;
	for(i=0;i<=NO_S;i++)
	{
		cout<<"FIRST["<<symbols[i]<<"]=";
		cout<<"{";
		j=0;
		while(first[i][j]!=END)
		{
			cout<<symbols[first[i][j]]<<",";
			j++;
		}
		cout<<"\b}"<<endl;
	}
	cout<<endl;
}

void create_follow()
{
	void find_follow(int);
	for(int i=0;i<NO_NT;i++)
	{
		if(!follow_d[i])
			find_follow(i);
	}
}

void find_follow(int X)
{
	bool check_added(int *,int);
	int count=-1;
	if(X==0)
		follow[X][++count]=DOLLAR;
	for(int i=0;i<NO_P;i++)
	{
		for(int j=0;grammer[i].right[j]!=END;j++)
		{
			if(X==grammer[i].right[j])
			{
				if(grammer[i].right[j+1]!=END)
				{
					for(int k=0;first[grammer[i].right[j+1]][k]!=END;k++)
					{
						if(!check_added(&follow[X][0],first[grammer[i].right[j+1]][k]))
							follow[X][++count]=first[grammer[i].right[j+1]][k];
					}
				}
				else
				{
					if(!follow_d[grammer[i].left])
						find_follow(grammer[i].left);
					for(int k=0;follow[grammer[i].left][k]!=END;k++)
					{
						if(!check_added(&follow[X][0],follow[grammer[i].left][k]))
							follow[X][++count]=follow[grammer[i].left][k];
					}
				}
			}
		}
	}
	follow[X][++count]=END;
	follow_d[X]=1;
}

void print_follow()
{
	int i,j;
	for(i=0;i<NO_NT;i++)
	{
		cout<<"FOLLOW["<<symbols[i]<<"]=";
		cout<<"{";
		j=0;
		while(follow[i][j]!=END)
		{
			cout<<symbols[follow[i][j]]<<",";
			j++;
		}
		cout<<"\b}"<<endl;
	}
	cout<<endl;
}

bool check_added(int *follow_X,int symbol)
{
	for(int i=0;*(follow_X+i)!=0;i++)
	{
		if(*(follow_X+i)==symbol)
			return true;
	}
	return false;
}

void create_table(int item_set[][21],int n)
{
	int i,j;
	i=j=0;
	for(i=0;i<MAX;i++)
		for(j=0;j<MAX;j++)
			slr_table[i][j].move=ERROR;
	//Build SLR TABLE
	for(i=0;i<=n;i++)
	{
		for(j=0;item_set[i][j]!=END;j+=2)
		{
			int prod_n=item_set[i][j];
			int dot_pos=item_set[i][j+1];
			int prod_s=grammer[prod_n].right[dot_pos];
			if(terminal(prod_s))
			{
				slr_table[i][prod_s].move=SHIFT;
				slr_table[i][prod_s].number=goto_v[i][j/2];
			}
			else if(prod_s==END && grammer[prod_n].left!=0)
			{
				for(int k=0;follow[grammer[prod_n].left][k]!=END;k++)
				{
					slr_table[i][follow[grammer[prod_n].left][k]].number=prod_n;
					slr_table[i][follow[grammer[prod_n].left][k]].move=REDUCE;
				}
			}
			else if(prod_n==0 && dot_pos==1)
					slr_table[i][DOLLAR].move=ACCEPT;
			else //prod_s is NON-TERMINAL
			{
				slr_table[i][prod_s].number=goto_v[i][j/2];
				slr_table[i][prod_s].move=NON_TERMINAL;
			}
		}
	}
}
void print_table(int n)
{
	int i,j;
	cout<<"\n\t\t\t\tSLR TABLE\n";
	cout<<setw(2)<<" ";
	for(i=1;i<=NO_S;i++)
	{
		cout<<setw(7)<<symbols[i];
	}
	for(i=0;i<=n;i++)
	{
		cout<<endl<<setw(2)<<i;
		for(j=1;j<=NO_S;j++)
		{
			switch(slr_table[i][j].move)
			{
			case ACCEPT:
				cout<<setw(7)<<"ACC";
				break;
			case ERROR:
				cout<<setw(7)<<" ";
				break;
			case SHIFT:
				cout<<setw(7)<<"S"<<slr_table[i][j].number;
				break;
			case REDUCE:
				cout<<setw(7)<<"R"<<slr_table[i][j].number;
				break;
			case NON_TERMINAL:
				cout<<setw(7)<<slr_table[i][j].number;
			}
		}
		cout<<endl;
	}
	cout<<endl;
}

void check_input()
{
	void push(int);
	int pop();
	int peek();
	char input[MAX];
	int input1[MAX];
	int i=0,len=-1,p;
	cout<<"Enter Input String\n";
	cin.get(input,MAX);
	while(input[i]!=NULL)
	{
		switch(input[i])
		{
		case 'i':
			if(input[++i]=='d')
			{
				input1[++len]=id;
			}
			break;
		case '*':
			input1[++len]=MP;
			break;
		case '+':
			input1[++len]=PS;
			break;
		case '(':
			input1[++len]=LP;
			break;
		case ')':
			input1[++len]=RP;
			break;
		default:
			cout<<"Invalid Input"<<endl;
			return;
		}
		++i;
	}
	input[i]='$';
	input[++i]='\0';
	input1[++len]=DOLLAR;
	push(0);
	cout<<"\t\t\tSLR PARSING MOVES"<<endl;
	cout<<setw(20)<<"Stack Top"<<setw(20)<<"INPUT"<<setw(20)<<"ACTION"<<endl;
	int done=0,state,sym;
	int ip=0,ip_ts=0;
	int move,number;
	char move_s[MAX];
	while(1)
	{
		state=peek();
		sym=input1[ip];
		move=slr_table[state][sym].move;
		number=slr_table[state][sym].number;
		switch(move)
		{
		case SHIFT:
			strcpy(move_s,"SHIFT");
			cout<<setw(20)<<peek()<<setw(20)<<&input[ip_ts]<<setw(20)<<move_s<<number<<endl;
			push(sym);
			push(number);
			ip++;
			if(sym==id)
				ip_ts+=2;
			else
				ip_ts++;
			break;
		case REDUCE:
			strcpy(move_s,"REDUCE BY P_NO ");
			cout<<setw(20)<<peek()<<setw(20)<<&input[ip_ts]<<setw(20)<<move_s<<number<<endl;
			for(p=1;p<=2*grammer[number].length;p++)
				pop();
			state=peek();
			push(grammer[number].left);
			push(slr_table[state][grammer[number].left].number);
			break;
		case ACCEPT:
			strcpy(move_s,"ACCEPT");
			cout<<setw(20)<<peek()<<setw(20)<<&input[ip_ts]<<setw(20)<<move_s<<number<<endl;
			cout<<"\nSTRING ACCEPTED\n";
			return;
		case ERROR:
			strcpy(move_s,"ERROR");
			cout<<setw(20)<<peek()<<setw(20)<<&input[ip_ts]<<setw(20)<<move_s<<number<<endl;
			cout<<"\nSTRING NOT ACCEPTED\n";
			return;
		}
	}
}

void push(int n)
{
	if(top<MAX-1)
		stck[++top]=n;
	else
	{
		cout<<"Stack Overflow";
		exit(0);
	}
}
int pop()
{
	if(top>=0)
		return stck[top--];
	else
	{
		cout<<"Stack underflow";
		exit(0);
	}
}

int peek()
{
	if(top>=0)
		return stck[top];
	else
	{
		cout<<"Stack underflow";
		exit(0);
	}
}
	
