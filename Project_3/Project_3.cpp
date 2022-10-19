#include<iostream>
#include<string>
#include<vector>
#include <iomanip>     
#include<map>
#include<sstream>
#include <fstream>
using namespace std;
int re[8];//記每個指令已調過
int sum_add_rs[8];
int sum_mul_rs[8];

int ADD_other_cycle = 2, MUL_cycle = 10, DIV_cycle = 40;
int ADD_RS = 3, MUL_RS = 2;
int ADD_OP = 0, ADDI_OP = 1, SUB_OP = 2, MUL_OP = 3, DIV_OP = 4;
int Issue_D = 1, WR_D = 1;//延遲
int Time = 0;
bool OK = true;//指令 == WRITEBACKS 結束程式
int Total_WRITEBACKS = 0; 
int PC = 0;//當前指令

int ZERO_REG = 0;
int RAT_0 = 1000;//空
int OperandAvailable = 1001;
int OperandInit = 1002;
//////////////////////////////////////////////////////////////////////////////////////////////////

class I {
public:
    int reg_wh_OP_wh1 = 0, OP_wh2 = 0, OP_wh3 = 0;
    string OP;//讀進來的add,addi,sub,mul,div
    int op = -1; // 對應數字
    int Issue_time = 0, WR_time = 0;
    int Exe_time_first = 0, Exe_time_end = 0;
    int wh = 0;
    I(int wh1, int wh2, int wh3, int OP,int wh_n)
    {
        reg_wh_OP_wh1 = wh1;
        OP_wh2 = wh2;
        OP_wh3 = wh3;
        op = OP;
        wh = wh_n;
       
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////

class RAT_list {
public:
    bool busy = false;
    int Qi = 0;
    RAT_list(int RAT_0)//空
    {
        Qi = RAT_0;
    }
};


/////////////////////////////////////////////////////////////////////////////////////////////////

class RS {
public:
    bool busy = false, resultReady = false;
    int instNum = 100000;
    int  Qj = 1, Qk = 1;
    int Vj = 0, Vk = 0;
    int Qj_n = 0, Qk_n = 0;
    int RS_D = 0, op = 0, A = 0;//RS_D延遲 A計算後的值
    int Issue_D = 0, WR_D = 0;//延遲
    RS(int OP, int RSoperandAvailable)
    {
        op = OP;
        Qj = RSoperandAvailable;
        Qk = RSoperandAvailable;
    }
};

struct B
{
    int RS = -1;
    int wh1 = 0, wh2 = 0;
    int op;
    int over = 0;
    int able = 1;

};
vector<B> Buf;
//vector<B> Buf2;
//////////////////////////////////////////////////////////////////////////////////////////////////
int string_to_int(string abc);
void F(string abc);//找
int Issue(vector<RAT_list>& RAT);
void Exe();
void WR(vector<RAT_list>& RAT);
void All_print(vector<RAT_list>& RAT);

//RS
RS ADD1(ADD_OP, OperandInit), ADD2(ADD_OP, OperandInit), ADD3(ADD_OP, OperandInit);
RS MUL1(MUL_OP, OperandInit), MUL2(MUL_OP, OperandInit);
vector<RS> RS_list = { ADD1, ADD2, ADD3, MUL1, MUL2 };

//RAT
RAT_list F0(RAT_0), F1(RAT_0), F2(RAT_0), F3(RAT_0), F4(RAT_0);
RAT_list F5(RAT_0), F6(RAT_0), F7(RAT_0), F8(RAT_0), F9(RAT_0);
RAT_list F10(RAT_0), F11(RAT_0), F12(RAT_0);

//指令


I
I0(1, 2, 1, ADDI_OP,0),
I1(1, 3, 4, SUB_OP,1),
I2(1, 2, 3, DIV_OP,2),
I3(2, 3, 4, MUL_OP,3),
I4(2, 4, 2, ADD_OP,4),
I5(4, 1, 2, ADDI_OP,5),
I6(5, 5, 5, MUL_OP,6),
I7(1, 4, 4, ADD_OP,7);
vector<I> IN = { I0,I1,I2,I3,I4,I5,I6,I7 };

//RF
vector<int> RF = { ZERO_REG,0,2,4,6,8,6,7,8,9,10,11,12 };

int main()
{
    for (int i = 0; i < 8; i++)
    {
        re[i] = 0;
        sum_add_rs[i] = 0;
        sum_mul_rs[i] = 0;

    }
    for (int i = 0; i <2; i++)
    {
        B b;
        Buf.push_back(b);
        
    }
    //RAT
    vector<RAT_list> RAT = { F0, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12 };

    do{
        Time += 1;

        Issue(RAT);
        Exe();
		WR(RAT);
        All_print(RAT);
       

        cout << endl;
        OK = false;

        if (Total_WRITEBACKS == IN.size())
        {
            OK = true;
        }
        cout << endl;
	}while(!OK);

    return 0;
}

int Issue(vector<RAT_list>& RAT)
{
    
    bool OK = false;
  
    if (PC >= IN.size())
    {
        return 0;
    }
    int n = IN[PC].op;
   
    if (n == ADD_OP)
    {
        for (int i = 0; i < 3; i++)
        {
            if (RS_list[i].busy == false)
            {
                RS_list[i].op = ADD_OP;
                n = i;
                PC++;
                OK = true;
                break;
            }
        }

        if (OK == false)
        {
            return 0;
        }
        else
        {
            if (RAT[IN[PC - 1].OP_wh2].Qi == RAT_0)
            {
                RS_list[n].Vj = RF[IN[PC - 1].OP_wh2];
                RS_list[n].Qj = OperandAvailable;
               
               
            }
            else
            {
                RS_list[n].Qj = RAT[IN[PC - 1].OP_wh2].Qi;
                RS_list[n].Qj_n = RAT[IN[PC - 1].OP_wh2].Qi;
            }

            if (RAT[IN[PC - 1].OP_wh3].Qi == RAT_0)
            {
                RS_list[n].Vk = RF[IN[PC - 1].OP_wh3];
                RS_list[n].Qk = OperandAvailable;
            }
            else
            {
                RS_list[n].Qk = RAT[IN[PC - 1].OP_wh3].Qi;
            }
        }
    }
    else if (n == ADDI_OP)
    {

        for (int i = 0; i < 3; i++) {
            if (RS_list[i].busy == false)
            {
                RS_list[i].op = ADDI_OP;
                n = i;
                PC++;
                OK = true;
                break;
            }
        }
        if (OK == false)
        {
            return 0;
        }
        else
        {
            if (RAT[IN[PC - 1].OP_wh2].Qi == RAT_0)
            {
                RS_list[n].Vj = RF[IN[PC - 1].OP_wh2];
                RS_list[n].Qj = OperandAvailable;
            }
            else
            {
                RS_list[n].Qj = RAT[IN[PC - 1].OP_wh2].Qi;
            }

            RS_list[n].Vk = IN[PC - 1].OP_wh3;
            RS_list[n].Qk = OperandAvailable;
        }
    }
    else if (n == SUB_OP)
    {

        for (int i = 0; i < 3; i++) {
            if (RS_list[i].busy == false) 
            {
                RS_list[i].op = SUB_OP;
                n = i;
                PC++;
                OK = true;
                break;
            }
        }
        if (OK == false)
        {
            return 0;
        }
        else
        {
            if (RAT[IN[PC - 1].OP_wh2].Qi == RAT_0)
            {
                RS_list[n].Vj = RF[IN[PC - 1].OP_wh2];
                RS_list[n].Qj = OperandAvailable;
            }
            else
            {
                RS_list[n].Qj = RAT[IN[PC - 1].OP_wh2].Qi;
            }

            if (RAT[IN[PC - 1].OP_wh3].Qi == RAT_0)
            {
                RS_list[n].Vk = RF[IN[PC - 1].OP_wh3];
                RS_list[n].Qk = OperandAvailable;
            }
            else
            {
                RS_list[n].Qk = RAT[IN[PC - 1].OP_wh3].Qi;
            }
        }
    }
    else if( n == MUL_OP)
    {
        for (int i = 3; i < 5; i++)
        {
            if (RS_list[i].busy == false) 
            {
                RS_list[i].op = MUL_OP;
                n = i;
                PC++;
                OK = true;
                break;
            }
        }
        if (OK == false)
        {
            return 0;
        }
        else
        {
            if (RAT[IN[PC - 1].OP_wh2].Qi == RAT_0)
            {
                RS_list[n].Vj = RF[IN[PC - 1].OP_wh2];
                RS_list[n].Qj = OperandAvailable;
            }
            else
            {
                RS_list[n].Qj = RAT[IN[PC - 1].OP_wh2].Qi;
            }

            if (RAT[IN[PC - 1].OP_wh3].Qi == RAT_0)
            {
                RS_list[n].Vk = RF[IN[PC - 1].OP_wh3];
                RS_list[n].Qk = OperandAvailable;
            }
            else
            {
                RS_list[n].Qk = RAT[IN[PC - 1].OP_wh3].Qi;
            }
        }
    }
    else if (n == DIV_OP)
    {
        for (int i = 3; i < 5; i++) 
        {
            if (RS_list[i].busy == false) 
            {
                RS_list[i].op = DIV_OP;
                n = i;
                PC += 1;
                OK = true;
                break;
            }
        }
        if (OK == false)
        {
            return 0;
        }
        else
        {
            if (RAT[IN[PC - 1].OP_wh2].Qi == RAT_0)
            {
                RS_list[n].Vj = RF[IN[PC - 1].OP_wh2];
                RS_list[n].Qj = OperandAvailable;
            }
            else
            {
                RS_list[n].Qj = RAT[IN[PC - 1].OP_wh2].Qi;
            }

            if (RAT[IN[PC - 1].OP_wh3].Qi == RAT_0)
            {
                RS_list[n].Vk = RF[IN[PC - 1].OP_wh3];
                RS_list[n].Qk = OperandAvailable;
            }
            else
            {
                RS_list[n].Qk = RAT[IN[PC - 1].OP_wh3].Qi;
            }
        }
    }

    RS_list[n].busy = true;
    RAT[IN[PC - 1].reg_wh_OP_wh1].Qi = n;
    RS_list[n].instNum = PC - 1;
    RS_list[n].Issue_D = 0;
    IN[PC - 1].Issue_time = Time;
    
    return 0;
    
   
}


void Exe()
{
    int busy_add_rs = 0;
    int busy_mul_rs = 0;
    int mul_num = 0, n = 0;
    for (int r=0;r< RS_list.size();r++)
    {
        
        if (RS_list[r].busy == true)
        {
           
            if (IN[RS_list[r].instNum].op == 0 || IN[RS_list[r].instNum].op == 1 || IN[RS_list[r].instNum].op == 2)
            {
                
               
                busy_add_rs++;
                for (int i = 0; i < r; i++)
                {

                    
                    if (busy_add_rs != 1)
                    {
                        /*if (IN[RS_list[i].instNum].wh < IN[RS_list[r].instNum].wh)
                            break;*/
                        if (sum_add_rs[RS_list[r].instNum] == 1)
                        {
                            RS_list[r].Issue_D = 1;
                           
                            break;
                        }
                        
                        if (IN[RS_list[i].instNum].op == 1)
                        {
                            if (IN[RS_list[i].instNum].reg_wh_OP_wh1 == IN[RS_list[r].instNum].reg_wh_OP_wh1 ||
                                IN[RS_list[i].instNum].reg_wh_OP_wh1 == IN[RS_list[r].instNum].OP_wh2 ||
                                IN[RS_list[i].instNum].OP_wh2 == IN[RS_list[r].instNum].reg_wh_OP_wh1 ||
                                IN[RS_list[i].instNum].OP_wh2 == IN[RS_list[r].instNum].OP_wh2 ||
                                sum_add_rs[RS_list[3].instNum] == 1 || sum_add_rs[RS_list[4].instNum] == 1)
                            {
                                
                                if (RS_list[i].resultReady == true && sum_add_rs[RS_list[r].instNum] == 0 ||
                                    sum_add_rs[3] == 1 && sum_add_rs[RS_list[r].instNum] == 0 ||
                                    sum_add_rs[4] == 1 && sum_add_rs[RS_list[r].instNum] == 0)
                                {
                                    
                                    RS_list[r].Issue_D = 0;
                                    if (sum_add_rs[RS_list[r].instNum] == 1 || sum_add_rs[3] == 1 || sum_add_rs[3] == 1)
                                    {
                                        
                                        RS_list[r].Issue_D = 1;
                                        break;
                                    }
                                    
                                    sum_add_rs[RS_list[r].instNum] = 1;
                                    break;
                                }
                                
                            }
                            
                        }
                        else
                        {
                            mul_num++;
                            if (IN[3].wh== IN[r].wh-1)
                            {
                                IN[4].Exe_time_first == Time;
                                RS_list[r].Issue_D = 1;
                                break;
                            }
                            if (sum_add_rs[RS_list[r].instNum] == 1)
                            {
                                RS_list[r].Issue_D = 1;
                               
                                break;
                            }
                            if (IN[RS_list[i].instNum].reg_wh_OP_wh1 == IN[RS_list[r].instNum].reg_wh_OP_wh1 ||
                                IN[RS_list[i].instNum].reg_wh_OP_wh1 == IN[RS_list[r].instNum].OP_wh2 ||
                                IN[RS_list[i].instNum].reg_wh_OP_wh1 == IN[RS_list[r].instNum].OP_wh3 ||
                                IN[RS_list[i].instNum].OP_wh2 == IN[RS_list[r].instNum].reg_wh_OP_wh1 ||
                                IN[RS_list[i].instNum].OP_wh2 == IN[RS_list[r].instNum].OP_wh2 ||
                                IN[RS_list[i].instNum].OP_wh2 == IN[RS_list[r].instNum].OP_wh3 ||
                                IN[RS_list[i].instNum].OP_wh3 == IN[RS_list[r].instNum].reg_wh_OP_wh1 ||
                                IN[RS_list[i].instNum].OP_wh3 == IN[RS_list[r].instNum].OP_wh2 ||
                                IN[RS_list[i].instNum].OP_wh3 == IN[RS_list[r].instNum].OP_wh3 ||
                                sum_add_rs[RS_list[3].instNum] == 1 || sum_add_rs[RS_list[4].instNum] == 1)
                            {
                                if (RS_list[i].resultReady == true && sum_add_rs[RS_list[r].instNum] == 0 ||
                                    sum_add_rs[3] == 1 && sum_add_rs[RS_list[r].instNum] == 0 ||
                                    sum_add_rs[4] == 1 && sum_add_rs[RS_list[r].instNum] == 0)
                                {
                                    RS_list[r].Issue_D = 0;
                                    if (sum_add_rs[RS_list[r].instNum] == 1 || sum_add_rs[3] == 1 || sum_add_rs[3] == 1)
                                    {
                                        RS_list[r].Issue_D = 1;
                                        break;
                                    }
                                    
                                    sum_add_rs[RS_list[r].instNum] = 1;
                                    break;
                                }
                            }
                            
                            
                        }

                        RS_list[r].Issue_D--;

                    }
                    else
                    {
                        
                        break;
                    }
                        
                    

                    
                }
                
                
            }
            else
            {
                
                busy_mul_rs++;
                for (int i = 3; i < r; i++)
                {
                    if (busy_mul_rs != 1)
                    {
                        /*if (IN[RS_list[i].instNum].wh < IN[RS_list[r].instNum].wh)
                        {
                            break;
                        }*/
                        if (sum_add_rs[RS_list[r].instNum] == 1)
                        {
                            RS_list[r].Issue_D = 1;
 
                            break;
                        }
                        if (IN[RS_list[i].instNum].reg_wh_OP_wh1 == IN[RS_list[r].instNum].reg_wh_OP_wh1 ||
                            IN[RS_list[i].instNum].reg_wh_OP_wh1 == IN[RS_list[r].instNum].OP_wh2 ||
                            IN[RS_list[i].instNum].reg_wh_OP_wh1 == IN[RS_list[r].instNum].OP_wh3 ||
                            IN[RS_list[i].instNum].OP_wh2 == IN[RS_list[r].instNum].reg_wh_OP_wh1 ||
                            IN[RS_list[i].instNum].OP_wh2 == IN[RS_list[r].instNum].OP_wh2 ||
                            IN[RS_list[i].instNum].OP_wh2 == IN[RS_list[r].instNum].OP_wh3 ||
                            IN[RS_list[i].instNum].OP_wh3 == IN[RS_list[r].instNum].reg_wh_OP_wh1 ||
                            IN[RS_list[i].instNum].OP_wh3 == IN[RS_list[r].instNum].OP_wh2 ||
                            IN[RS_list[i].instNum].OP_wh3 == IN[RS_list[r].instNum].OP_wh3)
                        {
                            if (RS_list[i].resultReady == true && sum_add_rs[RS_list[r].instNum] == 0)
                            {
                                RS_list[r].Issue_D = 0;
                                if (sum_add_rs[RS_list[r].instNum] == 1)
                                {
                                    RS_list[r].Issue_D = 1;
                                    break;
                                }
                                sum_add_rs[RS_list[r].instNum] = 1;

                                break;
                            }
                        }
                        RS_list[r].Issue_D--;

                    }
                    else
                    {
                        
                        break;
                    }
                }
            }
           
            
            //IN[4].Exe_time_first == Time;
            int k = 0;
            if (RS_list[r].Issue_D >= Issue_D)
            {
                if (RS_list[r].Qj == OperandAvailable && RS_list[r].Qk == OperandAvailable)
                {
                    if (IN[RS_list[r].instNum].Exe_time_first == 0 )
                    {
                        IN[RS_list[r].instNum].Exe_time_first = Time;
                        if (RS_list[r].op < 3)
                        {
                            if (Buf[0].over != 1)
                            {
                                Buf[0].op = RS_list[r].op;
                                Buf[0].RS = r;
                                Buf[0].wh1 = RS_list[r].Vj;
                                Buf[0].wh2 = RS_list[r].Vk;
                                Buf[0].able = 0;
                            }
                        }
                        else
                        {
                            if (Buf[1].over != 1)
                            {
                                Buf[1].op = RS_list[r].op;
                                Buf[1].RS = r;
                                Buf[1].wh1 = RS_list[r].Vj;
                                Buf[1].wh2 = RS_list[r].Vk;
                                Buf[1].able = 0;
                            }
                        }
                    }
                    RS_list[r].RS_D++;

                    if (RS_list[r].op == ADD_OP|| RS_list[r].op == ADDI_OP)
                    {
                        if (RS_list[r].RS_D == ADD_other_cycle)
                        {
                            RS_list[r].A = RS_list[r].Vj + RS_list[r].Vk;
                            RS_list[r].resultReady = true;
                            RS_list[r].RS_D = 0;
                            IN[RS_list[r].instNum].Exe_time_end = Time;
                            RS_list[r].Issue_D = 0;//延遲的時間
                            Buf[0].over = 1;
                        }
                    }
                    else if (RS_list[r].op == SUB_OP)
                    {
                        if (RS_list[r].RS_D == ADD_other_cycle)
                        {
                            RS_list[r].A = RS_list[r].Vj - RS_list[r].Vk;
                            RS_list[r].resultReady = true;
                            RS_list[r].RS_D = 0;
                            IN[RS_list[r].instNum].Exe_time_end = Time;
                            RS_list[r].Issue_D = 0;//延遲的時間
                            Buf[0].over = 1;
                        }
                    }
                    else if (RS_list[r].op == MUL_OP)
                    {
                        if (RS_list[r].RS_D == MUL_cycle)
                        {
                            RS_list[r].A = RS_list[r].Vj * RS_list[r].Vk;
                            RS_list[r].resultReady = true;
                            RS_list[r].RS_D = 0;
                            IN[RS_list[r].instNum].Exe_time_end = Time;
                            RS_list[r].Issue_D = 0;//延遲的時間
                            Buf[1].over = 1;
                        }
                    }
                    else if (RS_list[r].op == DIV_OP)
                    {
                        if (RS_list[r].RS_D == DIV_cycle)
                        {
                            RS_list[r].A = RS_list[r].Vj / RS_list[r].Vk;
                            RS_list[r].resultReady = true;
                            RS_list[r].RS_D = 0;
                            IN[RS_list[r].instNum].Exe_time_end = Time;
                            RS_list[r].Issue_D = 0;//延遲的時間
                            Buf[1].over = 1;
                        }
                    }

                }
            }
            else
            {
                RS_list[r].Issue_D++;//延遲，還沒準備好
            }
        }

    }

}

void WR(vector<RAT_list>& RAT)
{
    
    for(int r=0;r< RS_list.size();r++)
    {
        if (RS_list[r].resultReady)
        {
            if (RS_list[r].WR_D == WR_D)
            {
                if (IN[RS_list[r].instNum].WR_time == 0)
                {
                    IN[RS_list[r].instNum].WR_time = Time;
                }
                for (int x = 0; x < RF.size(); x++)
                {
                    if (RAT[x].Qi == r)
                    {
                        RF[x] = RS_list[r].A;
                        RAT[x].Qi = RAT_0;
                    }
                }
                
                for (int y = 0; y < RS_list.size(); y++)
                {
                    if (RS_list[y].Qj == r) 
                    {
                        RS_list[y].Vj = RS_list[r].A;
                        RS_list[y].Qj = OperandAvailable;
                    }
                    if (RS_list[y].Qk == r) 
                    {
                        RS_list[y].Vk = RS_list[r].A;
                        RS_list[y].Qk = OperandAvailable;
                    }
                }
                ///////////////////////////////////////////////////////////////////////////
                RS_list[r].resultReady = false;
                RS_list[r].busy = false;
                RS_list[r].Qj = OperandInit;
                RS_list[r].Qk = OperandInit;
                RS_list[r].Vj = 0;
                RS_list[r].Vk = 0;
                RS_list[r].WR_D = 0;
                Total_WRITEBACKS++;
            }
            else
            {
                RS_list[r].WR_D++;//延遲，還沒準備好
            }
        }
    }

}


void All_print(vector<RAT_list>& RAT){
    cout << "Cycle:" << Time << endl;
    cout << "___RF___" << endl;
    for (int i = 1,j=1; i <6 ; i++,j++)
    {
        if (RF[i] >= 10)
        {
            cout << "F" << j << "|" << RF[i] << "   |" << endl;
        }
        else
            cout << "F" << j << "|" << RF[i] << "    |" << endl;
    }
    cout <<"  ---------" << endl;
    cout << "___RAT___" << endl;
    for (int i = 1, j = 1; i < 6; i++, j++)
    {
        if (RAT[i].Qi >= 100)
        {
            cout << "F" << j << "|" << "   " << "|" << endl;
        }
        else
        {
            cout << "F" << j << "|" <<"RS" << RAT[i].Qi+1 << "|" << endl;
        }
    }
    cout << "  ---------" << endl;
    
    cout << "___RS________________________" << endl;
    for (int i = 1, j = 0; j < RS_list.size(); i++, j++)
    {
        cout << "RS" << i << "|";
        if (RS_list[j].op == 0 && RS_list[j].busy==true)
            cout << "+  " << " |";
        else if (RS_list[j].op == 1 && RS_list[j].busy == true)
            cout << "+  " << " |";
        else if (RS_list[j].op == 2 && RS_list[j].busy == true)
            cout << "-  " << " |";
        else if (RS_list[j].op == 3 && RS_list[j].busy == true)
            cout << "*  " << " |";
        else if (RS_list[j].op == 4 && RS_list[j].busy == true)
            cout << "/  " << " |";
        else
            cout << "   " << " |";
        if (RS_list[j].Vj == 0)
        {
            if (RS_list[j].Qj == 1002)
            {
                cout << "   " << " |";
            }
            else if (RS_list[j].Qj == 1001)
            {
                cout << "0  " << " |" ;
            }
            else
            {
                cout <<"RS" << RS_list[j].Qj+1 << " |";
            }
        }
        else
        {
            if (RS_list[j].Vj >= 10)
                cout << RS_list[j].Vj << " " << " |";
            else
                cout << RS_list[j].Vj << "   " << " |";
        }
        if (RS_list[j].Vk == 0)
        {
            if (RS_list[j].Qk == 1002)
            {
                cout << "   " << " |" << endl;
            }
            else if (RS_list[j].Qk == 1001)
            {
                cout << "0  " << " |" << endl;
            }
            else
                cout <<"RS" << RS_list[j].Qk+1 << " |" << endl;
        }
        else 
        {
            if (RS_list[j].Vk >= 10)
                cout << RS_list[j].Vk << " " << " |" << endl;
            else
                cout << RS_list[j].Vk << "  " << " |" << endl;
        }
        
        if (i == 3)
        {
            if (Buf[0].able == 1)//空了
            {
                cout << "BUFFER: empty" << endl;
            }
            else
            {
                cout << "BUFFER: (RS" << Buf[0].RS+1 << ")" << Buf[0].wh1;
                if (Buf[0].op == 0)
                    cout << "+";
                if (Buf[0].op == 1)
                    cout << "+";
                if (Buf[0].op == 2)
                    cout << "-";
                if (Buf[0].op == 3)
                    cout << "*";
                if (Buf[0].op == 4)
                    cout << "/";
                cout << Buf[0].wh2 << endl;
                if (Buf[0].over == 1)
                {
                    Buf[0].able = 1;
                    Buf[0].over = 0;
                }
            }

        }
        if (i == 5)
        {
            if (Buf[1].able == 1)//空了
            {
                cout << "BUFFER: empty" << endl;
            }
            else
            {
                cout << "BUFFER: (RS" << Buf[1].RS+1 << ")" << Buf[1].wh1;
                if (Buf[1].op == 0)
                    cout << "+";
                if (Buf[1].op == 1)
                    cout << "+";
                if (Buf[1].op == 2)
                    cout << "-";
                if (Buf[1].op == 3)
                    cout << "*";
                if (Buf[1].op == 4)
                    cout << "/";
                cout << Buf[1].wh2 << endl;
                if (Buf[1].over == 1)
                {
                    Buf[1].able = 1;
                    Buf[1].over = 0;
                }
            }
        }
    }
    cout << "  -----------------------------" << endl;
    

}
