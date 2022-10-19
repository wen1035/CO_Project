#include<iostream>
#include<string>
#include<vector>
#include<map>//hash table
#include<sstream>
#include <fstream>
using namespace std;

char P, R; //P為預測，R為真實 
string abc; // each input inst
int entry = 0, entry_num = 0;
int inst_size = 0;
int  PC = 0, PC_next = 0;
int history = 0;
map<string, int>LOOP_END; // Lable的位置

int LOOP_or_END = 0;
void up(char P, char R);
void ALL_cout();
void ten_to_two(int Number);
char outcome();
char predict();
void F(string abc);
struct I
{
	int wh = 0;
	string addi_beq_other;
	int addi_wh2_or_beq_wh1 = 0, beq_wh2 = 0;
	int reg_wh = 0, addi_int_wh3 = 0;

	string LOOP_orEND_wh;

};
vector<I>IN;


struct TNTN_3
{
	int history_TNTN = 0;
	string list2[8] = {"SN","SN", "SN", "SN", "SN", "SN", "SN", "SN"};
	int misprediction = 0;
};
vector<TNTN_3>List;

int reg[32] = {};



int main()
{
	cout << "Please input entry(entry>0):\n";
	cin >> entry_num;
	cin.ignore();
	inst_size = 0;
	//讀檔案
	ifstream ifs("1.txt", ios::in); //檔案以輸入方式開啟(檔案資料輸入到記憶體)
	if (!ifs.is_open())//判斷
	{
		cerr << "File is not open." << endl;
		exit(1);
	}
	while (getline(ifs, abc))
	{
		F(abc);
		inst_size++;
		if (abc == "END:")
			break;
	}
	//創空間
	for (int i = 0; i < entry_num; i++)
	{
		TNTN_3 T;
		List.push_back(T);//創entry_num個數的空間長度
	}

	while (PC < inst_size)
	{
		
		P = predict();//預測
		R = outcome();//真實
		if (P != R)
			List[entry].misprediction++;//錯
		ALL_cout();
		up(P, R);



		PC = PC_next;
		entry = PC % entry_num;
	}
	ifs.close(); //開啟的檔案使用完成後關閉
	return 0;
	
}


void F(string abc)
{
	stringstream ss(abc);
	string str;
	I in;
	in.wh = inst_size;
	ss >> str;
	if (str[0] != 'a' && str[0] != 'b')
	{
		str.erase(str.end() - 1);
		LOOP_END[str] = inst_size;
		
		inst_size--;
	}
	else
	{
		in.addi_beq_other = str;
		char a;
		int number = 0;
		if (str == "addi")
		{
			ss >> a >> number;
			in.reg_wh = number;
			ss >> a >> a >> number;
			in.addi_wh2_or_beq_wh1 = number;
			ss >> a >> number;
			in.addi_int_wh3 = number;
		}
		else //beq 
		{
			ss >> a >> number;
			in.addi_wh2_or_beq_wh1 = number;
			ss >> a >> a >> number;
			in.beq_wh2 = number;
			ss >> str;
			str.erase(str.begin());
			in.LOOP_orEND_wh = str;
		}
		IN.push_back(in);
	}
}

char predict()
{
	int L123 = List[entry].history_TNTN;
	if (List[entry].list2[L123] == "SN" || List[entry].list2[L123] == "WN1"|| List[entry].list2[L123] == "WN2"|| List[entry].list2[L123] == "WN3")
		return 'N';
	else
		return 'T';
}
char outcome()
{

	if (IN[PC].addi_beq_other == "addi")
	{
		//例R1=R0+0
		reg[IN[PC].reg_wh] = reg[IN[PC].addi_wh2_or_beq_wh1] + IN[PC].addi_int_wh3;
		PC_next = PC+1;//下一個
		return 'N';
	}
	else if (IN[PC].addi_beq_other == "beq")
	{
		if (reg[IN[PC].addi_wh2_or_beq_wh1] == reg[IN[PC].beq_wh2])//R1==r2 or R0==R0
		{
			PC_next = LOOP_END[IN[PC].LOOP_orEND_wh];//跳到END or LOOP
			return 'T';
		}
		else
		{
			PC_next = PC+1;//下一個
			return 'N';
		}
	}

}
void up(char P, char R)
{
	history = List[entry].history_TNTN;
	string OK = List[entry].list2[history];
	if (OK == "SN")
	{
		if (R == 'N')
		{

			OK = "SN";
		}
		else
		{
			OK = "WN1";
		}
	}
	else if (OK == "WN1")
	{
		if (R == 'N')
		{
			OK = "SN";
		}
		else
		{
			OK = "WN2";

		}
	}
	else if (OK == "WN2")
	{
		if (R == 'N')
		{
			OK = "WN1";
		}
		else
		{
			OK = "WN3";

		}
	}
	else if (OK == "WN3")
	{
		if (R == 'N')
		{
			OK = "WN2";
		}
		else
		{
			OK = "WT1";

		}
	}
	else if (OK == "WT1")
	{
		if (R == 'N')
		{
			OK = "WN3";
		}
		else
		{
			OK = "WT2";

		}

	}
	else if (OK == "WT2")
	{
		if (R == 'N')
		{
			OK = "WT1";
		}
		else
		{
			OK = "WT3";

		}

	}
	else if (OK == "WT3")
	{
		if (R == 'N')
		{
			OK = "WT2";
		}
		else
		{
			OK = "ST";

		}

	}
	else if (OK == "ST")
	{
		if (R == 'N')
		{
			OK = "WT3";
		}
		else
		{
			OK = "ST";

		}
	}
	List[entry].list2[history] = OK;

	history = (history % 4) * 2;

	if (R == 'T')
		history++;
	List[entry].history_TNTN = history;//紀錄
}

void ten_to_two(int Number)
{
	string w[3];
	if (Number >= 4)
	{
		w[2] = "1";
		Number -= 4;
	}
	else
	{
		w[2] = "0";
	}
	if (Number >= 2)
	{
		w[1] = "1";
		Number -= 2;
	}
	else
	{
		w[1] = "0";
	}
	if (Number >= 1)
		w[0] = "1";
	else
		w[0] = "0";
	for (int i = 2; i >= 0; i--)
		cout << w[i];


}

void ALL_cout()
{
	I ALL = IN[PC];


	cout << "entry: " << entry << "      ";
	if (ALL.addi_beq_other == "beq")
	{
		
		cout << "beq " << "R" << ALL.addi_wh2_or_beq_wh1 << " ,R" << ALL.beq_wh2 << " ," << ALL.LOOP_orEND_wh << endl;
	}
	else if (ALL.addi_beq_other == "addi")
	{
		cout << "addi " << "R" << ALL.reg_wh << " ,R" << ALL.addi_wh2_or_beq_wh1 << " ," << ALL.addi_int_wh3 << endl;
	}
	for (int i = 0; i < entry_num; i++)
	{
		cout << i << "  ( ";
		ten_to_two(List[i].history_TNTN);//000
		for (int j = 0; j < 8; j++)
		{
			cout << ", " << List[i].list2[j];
		}
		cout << " )";
		if (i == 0)
		{
			cout << " " << P << " " << R << " " << "misprediction: " << List[entry].misprediction << endl;
		}
		else
			cout << endl;

	}

	
	cout << "-----------------------------------------------------------------------------" << '\n';
}





